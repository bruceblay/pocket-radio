# Third-party components

Original Pocket Radio code: GPL-3.0-or-later, copyright 2026 Bruce Blay. The adapted M5Stack speaker-buffer code retains the MIT notice in M5STACK-LICENSE.

Pinned build dependencies:

- M5Unified 0.2.21: MIT; https://github.com/m5stack/M5Unified
- M5GFX 0.2.28: MIT; embedded font files retain their source notices, including GFXFF and IPA notices; https://github.com/m5stack/M5GFX
- ESP8266Audio 1.9.7: GPL-3.0-or-later for the MP3 wrapper; libmad decoder GPL-2.0-or-later. Other bundled codecs retain their respective notices; https://github.com/earlephilhower/ESP8266Audio/tree/1.9.7
- Arduino ESP32 2.0.17: core LGPL-2.1-or-later, with additional component licenses; https://github.com/espressif/arduino-esp32/tree/2.0.17
- ESP-IDF 4.4.7: Apache-2.0 and component-specific licenses. Vendor binary libraries retain Espressif's notices; https://github.com/espressif/esp-idf/tree/v4.4.7
- PlatformIO espressif32 6.12.0: Apache-2.0 build platform; https://github.com/platformio/platform-espressif32/tree/v6.12.0

Release preparation preserves the installed dependency files and recursively retrieves ESP-IDF v4.4.7 sources. These materials include component license notices. See release/SOURCE.md for archive contents and rebuild instructions. This inventory does not claim a comprehensive legal audit.
