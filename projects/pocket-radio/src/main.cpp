#include <M5Unified.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <AudioFileSource.h>
#include <AudioFileSourceBuffer.h>
#include <AudioGeneratorMP3.h>
#include <AudioOutput.h>
#include <memory>
#include <time.h>
#include <Preferences.h>
#include "ShakeDetector.h"
#include "stations.h"
#include "root_ca.h"
#include "WifiSetup.h"
#include "Version.h"

// No ICY metadata requested: decoder receives only MP3 bytes. TLS is verified.
class RadioSource : public AudioFileSource {
  WiFiClientSecure client;
  HTTPClient http;
  uint32_t position = 0;
public:
  bool open(const char* url) override {
    client.setCACert(radioRootCA);
    client.setHandshakeTimeout(8);
    http.setConnectTimeout(5000);
    http.setTimeout(3000);
    String target(url);
    bool connected = false;
    for (int redirects = 0; redirects <= 3; ++redirects) {
      if (!target.startsWith("https://") || !http.begin(client, target)) return false;
      const char* headers[] = {"Transfer-Encoding", "Content-Type", "Location"};
      http.collectHeaders(headers, 3);
      const int code = http.GET();
      Serial.printf("Stream HTTP status: %d\n", code);
      if (code == 301 || code == 302 || code == 303 || code == 307 || code == 308) {
        String next = http.header("Location");
        http.end();
        // Only HTTPS redirects; a new host must validate against our trust anchors.
        if (!next.startsWith("https://")) return false;
        target = next;
        continue;
      }
      String mime = http.header("Content-Type");
      if (code != HTTP_CODE_OK || !http.header("Transfer-Encoding").isEmpty() || !mime.startsWith("audio/mpeg")) {
        http.end(); return false;
      }
      connected = true;
      break;
    }
    if (!connected) return false;
    position = 0;
    return true;
  }
  uint32_t read(void* data, uint32_t len) override {
    auto* stream = http.getStreamPtr();
    if (!stream) return 0;
    // A stalled network can block here for up to three seconds.
    const uint32_t started = millis();
    while (!stream->available() && http.connected() && millis() - started < 3000) delay(1);
    return readNonBlock(data, len);
  }
  uint32_t readNonBlock(void* data, uint32_t len) override {
    auto* stream = http.getStreamPtr();
    if (!stream) return 0;
    const int available = stream->available();
    if (available <= 0) return 0;
    const int count = stream->read(static_cast<uint8_t*>(data), std::min(len, uint32_t(available)));
    if (count <= 0) return 0;
    position += count;
    return count;
  }
  bool isOpen() override { return http.connected(); }
  bool close() override { http.end(); return true; }
  uint32_t getPos() override { return position; }
  ~RadioSource() override { close(); }
};

// Triple-buffer pattern adapted from M5Stack's MIT-licensed WebRadio example.
// M5Unified owns ES8311/I2S/power configuration; do not initialize a second I2S driver.
class SpeakerOutput : public AudioOutput {
  int16_t samples[3][640] = {};
  size_t index = 0, used = 0;
public:
  uint32_t frames = 0;
  uint16_t peak = 0;
  bool begin() override { used = index = 0; frames = peak = 0; return true; }
  bool ConsumeSample(int16_t sample[2]) override {
    if (used == 640) { flush(); return false; }
    ++frames;
    peak = std::max(unsigned(peak), unsigned(abs(int(sample[0]))));
    samples[index][used++] = sample[0];
    samples[index][used++] = channels == 1 ? sample[0] : sample[1];
    return true;
  }
  void flush() override {
    if (!used) return;
    M5.Speaker.playRaw(samples[index], used, hertz, true, 1, 0);
    index = (index + 1) % 3;
    used = 0;
  }
  bool stop() override { M5.Speaker.stop(0); used = 0; return true; }
};

SpeakerOutput output;
std::unique_ptr<AudioFileSource> source;
std::unique_ptr<AudioFileSourceBuffer> buffer;
std::unique_ptr<AudioGenerator> decoder;
uint8_t streamBuffer[32768];
size_t station = 0;
constexpr int maxVolume = 184; // 72%, below M5Stack's 75% battery recommendation.
uint8_t volume = 128; // Start at 50%.
bool playing = true, dirty = true;
uint32_t retryAt = 0, wifiRetryAt = 0;
const char* status = "CONNECTING WI-FI";
constexpr uint16_t colors[] = {TFT_YELLOW, TFT_CYAN, TFT_GREEN, TFT_MAGENTA, TFT_ORANGE, TFT_WHITE};
constexpr size_t colorCount = sizeof(colors) / sizeof(colors[0]);
size_t colorIndex = 0;
WifiSetup wifiSetup;
uint32_t wifiLostAt = 0, bothPressedAt = 0;
uint32_t clockWaitAt = 0, clockRetryAt = 0;
bool waitingForClock = false;
bool suppressButtons = false;
Preferences preferences;
bool preferencesReady = false;
ShakeDetector shake;
int batteryLevel = -1;
bool charging = false;
uint32_t lastBatteryAt = 0, lastMotionAt = 0;

void updateBattery() {
  int level = M5.Power.getBatteryLevel();
  bool charge = M5.Power.isCharging() == m5::Power_Class::is_charging;
  if (level != batteryLevel || charge != charging) dirty = true;
  batteryLevel = level;
  charging = charge;
}
void updateMotion() {
  uint32_t now = millis();
  if (uint32_t(now - lastMotionAt) < 20) return;
  lastMotionAt = now;
  if (!M5.Imu.isEnabled() || !(M5.Imu.update() & m5::IMU_Class::sensor_mask_accel)) return;
  const auto data = M5.Imu.getImuData();
  if (shake.update(data.accel.x, data.accel.y, data.accel.z, now)) {
    colorIndex = (colorIndex + 1) % colorCount;
    if (preferencesReady) preferences.putUChar("color", colorIndex);
    dirty = true;
  }
}

void stopStream() {
  if (decoder) decoder->stop();
  decoder.reset();
  buffer.reset();
  source.reset();
  output.stop();
}
void draw() {
  auto& d = M5.Display;
  d.fillScreen(TFT_BLACK);
  const uint16_t ink = colors[colorIndex];
  d.setTextColor(ink, TFT_BLACK);
  d.setTextSize(1);
  if (wifiSetup.active) {
    // Three short steps; reserve large type for what the user must read/type.
    d.setCursor(8, 6); d.print("1. JOIN WI-FI ON PHONE");
    d.setTextSize(2);
    d.setCursor(8, 19); d.print(wifiSetup.network);
    d.setTextSize(1);
    d.setCursor(8, 43); d.print("2. PASSWORD");
    d.setTextSize(2);
    d.setCursor(8, 56); d.print(wifiSetup.password);
    d.setTextSize(1);
    d.setCursor(8, 80); d.print("3. OPEN IN PHONE BROWSER");
    d.setTextSize(2);
    d.setCursor(8, 93); d.print("http://192.168.4.1");
    d.setTextSize(1);
    if (strcmp(wifiSetup.screenStatus, "JOIN FROM YOUR PHONE") != 0) {
      d.setCursor(8, 120); d.print(wifiSetup.screenStatus);
    }
    dirty = false;
    return;
  }
  d.setCursor(8, 7); d.print("POCKET RADIO");
  d.setCursor(90, 7); d.printf("VOL %02u", unsigned(volume * 100 / 255));
  d.setCursor(148, 7);
  if (batteryLevel < 0) d.print("BAT --");
  else d.printf("BAT ~%d%%%s", batteryLevel, charging ? "+" : "");
  d.drawFastHLine(8, 23, 224, ink);
  d.setTextSize(2);
  d.setCursor(8, 33); d.print(stations[station].name);
  d.setTextSize(1);
  d.setCursor(8, 57); d.print(stations[station].mood);
  size_t dialPosition = 0, dialCount = wifiSetup.selection.count();
  for (size_t i = 0; i < stationCount; ++i) {
    if (!wifiSetup.selection.includes(i)) continue;
    int x = 14 + (dialCount == 1 ? 0 : dialPosition * 210 / (dialCount - 1));
    ++dialPosition;
    d.drawFastVLine(x, 77, 12, ink);
    if (i == station) d.fillTriangle(x - 4, 72, x + 4, 72, x, 77, ink);
  }
  d.setCursor(8, 96); d.print(status);
  d.setCursor(8, 107); d.print(WiFi.status() != WL_CONNECTED || time(nullptr) < 1700000000 ? "HOLD A+B FOR WI-FI SETUP" : "SHAKE COLOR / A+B WI-FI SETUP");
  d.setCursor(8, 119); d.print("A NEXT/HOLD STOP  B VOL/HOLD -");
  dirty = false;
}
void decoderStatus(void*, int code, const char* message) {
  // Joining a live MP3 can require resynchronization. Avoid flooding USB and
  // delaying audio with a log line for every decoder retry.
  static uint32_t lastLog = 0;
  if (uint32_t(millis() - lastLog) < 1000) return;
  lastLog = millis();
  Serial.printf("Decoder %d: %s\n", code, message);
}
void startStream() {
  Serial.printf("Tune: %s\n", stations[station].name);
  stopStream();
  status = "TUNING...";
  draw();
  source.reset(new RadioSource());
  if (source->open(stations[station].url)) {
    buffer.reset(new AudioFileSourceBuffer(source.get(), streamBuffer, sizeof(streamBuffer)));
    decoder.reset(new AudioGeneratorMP3());
    decoder->RegisterStatusCB(decoderStatus, nullptr);
    if (decoder->begin(buffer.get(), &output)) {
      status = "LIVE / INTERNET RADIO";
      dirty = true;
      return;
    }
  }
  stopStream();
  status = "STREAM FAILED / RETRY 10S";
  retryAt = millis() + 10000;
  dirty = true;
}
void setup() {
  auto cfg = M5.config();
  cfg.internal_spk = true;
  cfg.internal_imu = true;
  cfg.internal_mic = false;
  M5.begin(cfg);
  Serial.begin(115200);
  preferencesReady = preferences.begin("pocket-radio", false);
  if (preferencesReady) colorIndex = preferences.getUChar("color", 0) % colorCount;
  updateBattery();
  lastBatteryAt = millis();
  M5.Display.setRotation(1);
  M5.Display.setBrightness(90);
  M5.Speaker.setVolume(volume);
  M5.BtnA.setHoldThresh(600);
  M5.BtnB.setHoldThresh(600);
  wifiSetup.begin();
  station = wifiSetup.selection.first();
  wifiLostAt = millis();
  wifiRetryAt = millis() + 15000;
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  draw();
}
void loop() {
  M5.update();
  // Setup mode also needs diagnostics; never print network names or passwords.
  if (wifiSetup.active && Serial.available()) {
    if (Serial.read() == '?') Serial.printf("Pocket Radio %s | SETUP | %s | stations=%u\n",
      radioVersion, wifiSetup.screenStatus, unsigned(wifiSetup.selection.count()));
  }
  if (wifiSetup.active) {
    const char* before = wifiSetup.screenStatus;
    if (wifiSetup.loop()) {
      if (!wifiSetup.selection.includes(station)) station = wifiSetup.selection.first();
      playing = true; retryAt = 0; wifiRetryAt = millis() + 15000; wifiLostAt = millis();
      suppressButtons = true; bothPressedAt = millis();
      status = "CONNECTING...";
      configTime(0, 0, "pool.ntp.org", "time.nist.gov");
      dirty = true;
    }
    if (before != wifiSetup.screenStatus) dirty = true;
    if (dirty) draw();
    delay(1);
    return;
  }
  // Stop decoding while recognizing the setup chord so network reads cannot
  // delay its timer. Suppress individual button actions through both releases.
  if (M5.BtnA.isPressed() && M5.BtnB.isPressed()) {
    if (!suppressButtons) { suppressButtons = true; bothPressedAt = millis(); stopStream(); }
    if (uint32_t(millis() - bothPressedAt) >= 1500) {
      wifiSetup.open(); dirty = true; draw(); return;
    }
  } else if (suppressButtons) {
    // An interrupted chord must start its hold timer again.
    bothPressedAt = millis();
  }
  if (suppressButtons) {
    if (!M5.BtnA.isPressed() && !M5.BtnB.isPressed()) suppressButtons = false;
    delay(1); return;
  }
  // USB diagnostics: l=Lounge, t=Tiki, ?=status. No credentials are printed.
  if (Serial.available()) {
    char command = Serial.read();
    if (command == 'l' || command == 't') {
      stopStream(); station = command == 'l' ? 1 : 0;
      if (!wifiSetup.selection.includes(station)) station = wifiSetup.selection.first();
      playing = true; retryAt = 0; dirty = true;
    }
    if (command == '?') {
      Serial.printf("Pocket Radio %s | %s | %s | frames=%u peak=%u heap=%u psram=%u\n",
        radioVersion, stations[station].name, status, output.frames, output.peak, ESP.getFreeHeap(), ESP.getFreePsram());
    }
  }
  updateMotion();
  if (uint32_t(millis() - lastBatteryAt) >= 10000) {
    lastBatteryAt = millis();
    updateBattery();
  }
  if (M5.BtnA.wasHold()) {
    playing = !playing;
    stopStream();
    status = playing ? "CONNECTING..." : "STOPPED";
    retryAt = 0;
    dirty = true;
  } else if (M5.BtnA.wasClicked()) {
    station = wifiSetup.selection.next(station);
    stopStream();
    retryAt = 0;
    dirty = true;
  }
  if (M5.BtnB.wasHold()) {
    volume = volume >= 16 ? volume - 16 : 0;
    dirty = true;
  } else if (M5.BtnB.wasClicked()) {
    volume = std::min(maxVolume, volume + 16);
    dirty = true;
  }
  if (dirty) M5.Speaker.setVolume(volume);
  if (WiFi.status() == WL_CONNECTED) wifiLostAt = millis();
  if (WiFi.status() != WL_CONNECTED || time(nullptr) >= 1700000000) waitingForClock = false;
  if (playing && wifiSetup.hasSaved()) {
    if (WiFi.status() != WL_CONNECTED) {
      if (decoder) stopStream();
      if (strcmp(status, "CONNECTING WI-FI") != 0) dirty = true;
      status = "CONNECTING WI-FI";
      if (uint32_t(millis() - wifiLostAt) >= 45000) {
        wifiSetup.open(); dirty = true; draw(); return;
      }
      if (int32_t(millis() - wifiRetryAt) >= 0) {
        wifiRetryAt = millis() + 15000;
        WiFi.reconnect();
      }
    } else if (time(nullptr) < 1700000000) {
      if (!waitingForClock) {
        waitingForClock = true;
        clockWaitAt = clockRetryAt = millis();
      }
      const char* clockStatus = uint32_t(millis() - clockWaitAt) < 30000
        ? "SYNCING CLOCK..." : "CLOCK FAILED / CHECK WI-FI";
      if (strcmp(status, clockStatus) != 0) dirty = true;
      status = clockStatus;
      if (uint32_t(millis() - clockRetryAt) >= 30000) {
        clockRetryAt = millis();
        configTime(0, 0, "pool.ntp.org", "time.nist.gov");
      }
    } else if (!decoder && int32_t(millis() - retryAt) >= 0) {
      startStream();
    } else if (decoder && !decoder->loop()) {
      stopStream();
      status = "SIGNAL LOST / RETRY 10S";
      retryAt = millis() + 10000;
      dirty = true;
    }
  }
  if (dirty) draw();
  delay(1);
}
