# StickS3 research — 2026-09-09

## Hardware and tools

The [official hardware reference](https://docs.m5stack.com/en/core/StickS3) documents an ESP32-S3 with 8 MB flash and 8 MB octal PSRAM, 2.4 GHz Wi-Fi, a 135 × 240 LCD, two programmable buttons, an ES8311 audio codec and onboard speaker. The 250 mAh battery makes power use a practical constraint. Keep speaker volume below 75% on battery, per M5Stack; Pocket Radio caps it at 184/255. USB power is useful for development.

Use a USB-C data cable and Arduino C++. Arduino IDE, PlatformIO and ESP-IDF are supported. PlatformIO gives each toy a reproducible configuration. The vendor's generic S3 board configuration requires octal PSRAM and 8 MB partitions; its board name alone does not describe all StickS3 settings.

[Arduino upload instructions](https://docs.m5stack.com/en/arduino/m5sticks3/program) and [speaker example](https://docs.m5stack.com/en/arduino/m5sticks3/speaker) specify M5Stack board package >=3.2.5, M5Unified >=0.2.12 and M5GFX >=0.2.18 for Arduino IDE. These are not PlatformIO package version numbers. Our PlatformIO environment uses espressif32 6.12.0 and pins newer M5 libraries explicitly.

Use M5Unified for device detection, display, buttons, codec and speaker power. [Power documentation](https://docs.m5stack.com/en/arduino/m5sticks3/m5pm1) explains that the amplifier is managed through the power controller. Avoid copying GPIO/I2S settings from StickC or CoreS3 projects. Do not enable the microphone for this playback-only app.

## Radio implementation

[M5Stack's web-radio example](https://github.com/m5stack/M5Unified/blob/master/examples/Advanced/WebRadio_with_ESP8266Audio/WebRadio_with_ESP8266Audio.ino) connects [ESP8266Audio](https://github.com/earlephilhower/ESP8266Audio) decoding to M5Unified's `Speaker.playRaw()` with three PCM buffers. The output adapter here follows that example. Retain its MIT notice.

ESP8266Audio 1.9.7 is pinned for the Arduino 2.x framework supplied by this PlatformIO release. Its ICY source uses a plain WiFiClient. Pocket Radio instead supplies a small verified-HTTPS source to the MP3 decoder and requests no ICY metadata. It does not disable certificate validation. A USERTrust RSA root from the Mozilla CA store is embedded for the current SomaFM certificate chain; the root must be updated if the broadcaster changes CA. NTP clock sync precedes TLS. Do not assume every new station shares this trust root.

The first version accepts direct MP3 responses, not station web pages, PLS/M3U playlists, HLS, AAC, Ogg or chunked transfer encoding. There is a 32 KiB compressed-audio buffer. Connection and read operations are synchronous, so buttons can respond slowly during network failures. A separate audio worker would be a worthwhile next improvement after hardware validation.

Preset endpoints come from the official [Groove Salad](https://somafm.com/groovesalad.pls), [Drone Zone](https://somafm.com/dronezone.pls) and [DEF CON Radio](https://somafm.com/defcon.pls) playlists. Playlist retrieval succeeded during research. Stream reachability is separately tested and is not guaranteed by a published URL. [Support SomaFM](https://somafm.com/support/).

## Community

Useful contributions: tested station presets, device setup fixes, reproducible audio bug reports, better reconnection, accessible controls and photos/video of real hardware. Keep credentials out of commits and release binaries. Record exact library versions and distinguish compilation from a listening test.

Before distributing firmware, retain dependency notices and comply with ESP8266Audio's GPL licensing, including corresponding source. No top-level license is selected yet for the owner's original work; choose one before inviting redistribution.

## Curated dial update

The original MP3-only design above is historical. See `station-options.md` and the radio README for the six-station dial and narrow BBC HLS/AAC implementation. The trusted roots now cover the selected broadcaster chains, and MP3 sources follow bounded HTTPS redirects. BBC segment fetching runs in a worker using PSRAM.

## Current lineup: BBC replaced

At the user's request, replaced BBC Radio 1 with Illinois Street Lounge (display: Illinois St Lounge), a vintage lounge/exotica station that complements Tiki Time. Official playlist: https://somafm.com/illstreet.pls. The selected ice2 HTTPS endpoint returned HTTP 200 and MP3 audio bytes. Removed the HLS transport, TS parser and AAC wrapper from the project; all six presets now use the common MP3 playback path. Earlier BBC notes above are historical.
