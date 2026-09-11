#pragma once
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <Preferences.h>
#include "PortalInput.h"
#include "StationSelection.h"
#include "stations.h"
#include "SetupPage.h"
#include "Version.h"
static_assert(stationCount < 32, "Station mask supports up to 31 presets");

// All portal assets are embedded: no internet, CDN, or phone app required.
class WifiSetup {
  struct Credentials { char ssid[33]; char password[65]; } saved{}, candidate{};
  Preferences store;
  WebServer server{80};
  DNSServer dns;
  bool storageReady = false, connecting = false, startPending = false;
  uint32_t attemptAt = 0, closeAt = 0;
  String token;
  String message;
  void page(int code = 200) {
    String html = setupPageStart;
    if (message.length()) html += "<p class=\"status\" role=\"status\">" + portalEscape(message) + "</p>";
    if (!connecting && !closeAt) {
      html += "<h2>" + String(hasSaved() ? "Change Wi-Fi" : "Connect to Wi-Fi") + "</h2><form action=\"/connect\" method=\"post\"><input type=\"hidden\" name=\"token\" value=\"" + token + "\">";
      html += setupNetworkFields;
      html += "<form id=\"stations-form\" action=\"/stations\" method=\"post\"><input type=\"hidden\" name=\"token\" value=\"" + token + "\"><fieldset><legend>Your stations</legend><p class=\"note\">The original six start selected. Add more below, or uncheck any you don't want. Keep at least one.</p>";
      for (size_t i = 0; i < stationCount; ++i) {
        if (i == defaultStationCount) html += "<h3>More to explore</h3>";
        html += "<label class=\"station\"><input type=\"checkbox\" name=\"s" + String(i) + "\" value=\"1\"";
        if (selection.includes(i)) html += " checked";
        html += "><span>" + portalEscape(String(stations[i].name)) + "<small>" + portalEscape(String(stations[i].mood)) + "</small></span></label>";
      }
      html += "</fieldset><button>Save stations</button><p id=\"stations-status\" class=\"note\" role=\"status\"></p></form>";
      if (hasSaved()) html += "<form action=\"/resume\" method=\"post\"><input type=\"hidden\" name=\"token\" value=\"" + token + "\"><button class=\"secondary\">Return to radio</button><p class=\"note\">Uses your saved Wi-Fi. Save station changes first.</p></form>";
      if (hasSaved()) html += "<form action=\"/forget\" method=\"post\"><input type=\"hidden\" name=\"token\" value=\"" + token + "\"><button class=\"secondary\">Forget saved Wi-Fi</button></form>";
      html += setupPageScript;
    }
    html += "<p class=\"note\">This page comes directly from your radio. Settings stay on the device. Once connected, return your phone to its usual Wi-Fi.</p><p class=\"note\">Pocket Radio " + String(radioVersion) + "</p></body></html>";
    server.sendHeader("Cache-Control", "no-store");
    server.sendHeader("X-Frame-Options", "DENY");
    if (connecting) server.sendHeader("Refresh", "3; url=/");
    server.send(code, "text/html; charset=utf-8", html);
  }
  bool authorized() {
    if (server.arg("token") == token) return true;
    server.send(403, "text/plain", "Reload the setup page and try again.");
    return false;
  }
public:
  StationSelection selection{stationCount, defaultStationMask};
  bool active = false;
  String network, password;
  const char* screenStatus = "JOIN FROM YOUR PHONE";
  bool hasSaved() const { return saved.ssid[0] != 0; }
  void begin() {
    storageReady = store.begin("radio-wifi", false);
    if (storageReady && store.getBytesLength("config") == sizeof(saved)) {
      store.getBytes("config", &saved, sizeof(saved));
      saved.ssid[32] = 0; saved.password[64] = 0;
    }
    if (storageReady) selection.set(store.getUInt("stations", selection.mask()));
    WiFi.persistent(false);
    WiFi.setAutoReconnect(true);
    WiFi.mode(WIFI_STA);
    WiFi.setSleep(false);
    if (hasSaved()) WiFi.begin(saved.ssid, saved.password);
    else open();
  }
  void open() {
    if (active) return;
    connecting = startPending = false; closeAt = 0;
    message = ""; screenStatus = "JOIN FROM YOUR PHONE";
    char suffix[5], key[9], nonce[33];
    snprintf(suffix, sizeof(suffix), "%04X", unsigned(ESP.getEfuseMac() & 0xffff));
    snprintf(key, sizeof(key), "%08X", unsigned(esp_random()));
    snprintf(nonce, sizeof(nonce), "%08X%08X%08X%08X", unsigned(esp_random()), unsigned(esp_random()), unsigned(esp_random()), unsigned(esp_random()));
    network = String("PocketRadio-") + suffix; password = key; token = nonce;
    WiFi.disconnect(false, true);
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAPConfig(IPAddress(192,168,4,1), IPAddress(192,168,4,1), IPAddress(255,255,255,0));
    active = true;
    if (!WiFi.softAP(network.c_str(), password.c_str())) {
      screenStatus = "SETUP FAILED / RESTART"; return;
    }
    dns.start(53, "*", WiFi.softAPIP());
    // Re-registering routes on every opening would accumulate handlers.
    static bool routesReady = false;
    if (!routesReady) {
      routesReady = true;
      server.on("/", HTTP_GET, [this] { page(); });
      const char* trackedHeaders[] = {"Accept"};
      server.collectHeaders(trackedHeaders, 1);
      server.on("/networks", HTTP_GET, [this] {
        int count = WiFi.scanComplete();
        String json = String("{\"scanning\":") + (count == WIFI_SCAN_RUNNING ? "true" : "false") + ",\"networks\":[";
        int included = 0;
        for (int i = 0; i < count && included < 32; ++i) {
          String ssid = WiFi.SSID(i);
          if (ssid.isEmpty()) continue;
          bool duplicate = false;
          for (int j = 0; j < i; ++j) if (WiFi.SSID(j) == ssid) { duplicate = true; break; }
          if (duplicate) continue;
          if (included++) json += ',';
          json += "{\"ssid\":\"" + portalJsonEscape(ssid) + "\",\"rssi\":" + String(WiFi.RSSI(i)) + ",\"open\":" + (WiFi.encryptionType(i) == WIFI_AUTH_OPEN ? "true" : "false") + "}";
        }
        json += "]}";
        server.sendHeader("Cache-Control", "no-store");
        server.send(200, "application/json", json);
      });
      server.on("/scan", HTTP_POST, [this] {
        if (!authorized()) return;
        if (connecting || closeAt) { server.send(409, "text/plain", "Connection in progress"); return; }
        if (WiFi.scanComplete() != WIFI_SCAN_RUNNING) { WiFi.scanDelete(); WiFi.scanNetworks(true); }
        server.send(202, "text/plain", "Scanning");
      });
      server.on("/stations", HTTP_POST, [this] {
        if (!authorized()) return;
        uint32_t mask = 0;
        for (size_t i = 0; i < stationCount; ++i) if (server.arg(String("s") + i) == "1") mask |= uint32_t(1) << i;
        int code = 200;
        if (connecting || closeAt) { message = "Connection in progress. Reopen setup to edit stations."; code = 409; }
        else if (!mask) { message = "Choose at least one station."; code = 400; }
        else if (!storageReady || store.putUInt("stations", mask) != sizeof(uint32_t)) { message = "Couldn't save stations. Please try again."; code = 500; }
        else { selection.set(mask); message = "Stations saved. Return to the radio when you're ready."; }
        if (server.header("Accept") == "application/json") {
          server.sendHeader("Cache-Control", "no-store");
          server.send(code, "application/json", "{\"message\":\"" + portalJsonEscape(message) + "\"}");
        } else page(code);
      });
      server.on("/resume", HTTP_POST, [this] {
        if (!authorized()) return;
        if (connecting || closeAt) { page(); return; }
        if (!hasSaved()) { message = "Connect to Wi-Fi first."; page(400); return; }
        candidate = saved;
        connecting = startPending = true;
        message = "Returning to your saved Wi-Fi… Watch the radio screen.";
        screenStatus = "CONNECTING SAVED WI-FI";
        page();
      });
      server.on("/connect", HTTP_POST, [this] {
        if (!authorized()) return;
        if (connecting || closeAt) { page(); return; }
        String ssid = server.arg("ssid"), pass = server.arg("password");
        if (!portalCredentialsValid(ssid, pass)) {
          message = "Enter a network name (up to 32 bytes) and a valid Wi-Fi password (8–63 characters, or 64 hex digits)."; page(400); return;
        }
        memset(&candidate, 0, sizeof(candidate));
        ssid.toCharArray(candidate.ssid, sizeof(candidate.ssid));
        pass.toCharArray(candidate.password, sizeof(candidate.password));
        connecting = startPending = true;
        message = "Connecting… Watch the radio screen. If your phone disconnects, rejoin the setup network to check progress.";
        screenStatus = "TESTING YOUR WI-FI...";
        page();
      });
      server.on("/forget", HTTP_POST, [this] {
        if (!authorized()) return;
        if (connecting || closeAt) { page(); return; }
        if (storageReady && (!store.isKey("config") || store.remove("config"))) {
          memset(&saved, 0, sizeof(saved));
          message = "Saved Wi-Fi forgotten. Enter a network to start again.";
        } else message = "Couldn't erase settings. Restart the radio and try again.";
        page();
      });
      server.onNotFound([this] {
        server.sendHeader("Location", "http://192.168.4.1/", true);
        server.send(302, "text/plain", "Open http://192.168.4.1/");
      });
    }
    server.begin();
    WiFi.scanDelete();
    WiFi.scanNetworks(true);
  }
  // Returns true once the portal closes after saving a successful connection.
  bool loop() {
    if (!active) return false;
    dns.processNextRequest(); server.handleClient();
    if (startPending && WiFi.scanComplete() != WIFI_SCAN_RUNNING) {
      startPending = false;
      WiFi.scanDelete();
      WiFi.disconnect(false, true);
      WiFi.begin(candidate.ssid, candidate.password);
      attemptAt = millis();
    }
    if (connecting && !startPending && WiFi.status() == WL_CONNECTED && WiFi.SSID() == candidate.ssid && uint32_t(millis() - attemptAt) >= 500) {
      connecting = false;
      if (storageReady && store.putBytes("config", &candidate, sizeof(candidate)) == sizeof(candidate)) {
        saved = candidate;
        message = "Connected! Settings saved. Your radio will start playing; you can close this page.";
        screenStatus = "CONNECTED! STARTING...";
        closeAt = millis() + 5000;
      } else {
        message = "Connected, but couldn't save settings. Restart the radio and try again.";
        screenStatus = "SAVE FAILED / TRY AGAIN";
        WiFi.disconnect(false, true);
      }
      memset(&candidate, 0, sizeof(candidate));
    } else if (connecting && !startPending && uint32_t(millis() - attemptAt) >= 25000) {
      connecting = false;
      WiFi.disconnect(false, true);
      memset(&candidate, 0, sizeof(candidate));
      message = "Couldn't connect. Check the network name and password, and make sure it is 2.4 GHz. Previous saved settings are unchanged.";
      screenStatus = "FAILED / RETRY ON PHONE";
    }
    if (closeAt && int32_t(millis() - closeAt) >= 0) {
      dns.stop(); server.stop(); WiFi.scanDelete();
      WiFi.softAPdisconnect(false); WiFi.mode(WIFI_STA); WiFi.setSleep(false);
      active = false; closeAt = 0; password = ""; token = "";
      return true;
    }
    return false;
  }
};
