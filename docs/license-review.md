# Pocket Radio distribution review — September 11, 2026

Bruce approved GPL-3.0-or-later for original Pocket Radio code on September 11, 2026. The license text is in the repository root and firmware project. Upstream files retain their original notices. Release materials include the project source, full installed dependency inputs and their notices, plus ESP-IDF v4.4.7 with recursive submodules.

Read directly from the installed, pinned source:

| Component | Evidence | Next release work |
| --- | --- | --- |
| M5Unified 0.2.21 | Top-level MIT LICENSE | Preserve notice and source/version reference |
| M5GFX 0.2.28 | Top-level MIT LICENSE | Preserve notice; review bundled fonts/assets and their notices |
| ESP8266Audio 1.9.7 | LICENSE contains GPLv3; AudioGeneratorMP3.cpp says version 3 or later | Preserve notices and provide corresponding source/build materials |
| libmad used by AudioGeneratorMP3 | decoder.c says GPL version 2 or later | Preserve notices and source; top-level COPYing alone doesn't describe the later-version option |
| Arduino ESP32 2.0.17 | Arduino.h says LGPL version 2.1 or later | Preserve notices, exact source references, and review distributed SDK components/build materials |
| Adapted M5Stack web-radio speaker buffer | Local M5STACK-LICENSE | Keep upstream attribution and license |

This inventory is preliminary. It does not establish that every statically linked SDK, font, or codec component has been reviewed. ESP8266Audio ships additional codecs even though this app uses MP3; distinguish linked code from merely installed source using the final link map.

The release distributes these materials separately: project source, dependency build inputs (M5Unified, M5GFX, ESP8266Audio, Arduino framework and PlatformIO platform), and ESP-IDF v4.4.7 source with submodules. Dependency inputs preserve the exact installed SDK binaries and headers used by PlatformIO, alongside available source and notices. See release/SOURCE.md for rebuilding and archive contents. No claim of a comprehensive legal audit is made.

Primary source repositories: [M5Unified](https://github.com/m5stack/M5Unified), [M5GFX](https://github.com/m5stack/M5GFX), [ESP8266Audio 1.9.7](https://github.com/earlephilhower/ESP8266Audio/tree/1.9.7), [Arduino ESP32 2.0.17](https://github.com/espressif/arduino-esp32/tree/2.0.17).
