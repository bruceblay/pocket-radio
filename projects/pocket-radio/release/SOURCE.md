# Source and build materials for 0.1.0

Download all three source/build archives from the same GitHub release:

- `pocket-radio-0.1.0-source.tar.gz`: tagged project source, license, and build/package scripts.
- `pocket-radio-0.1.0-dependency-inputs.tar.gz`: full installed M5Unified 0.2.21, M5GFX 0.2.28, ESP8266Audio 1.9.7, Arduino ESP32 2.0.17 framework package, and PlatformIO espressif32 6.12.0 platform. Source-level and bundled component license notices are preserved. The framework includes the vendor's prebuilt SDK libraries and headers.
- `pocket-radio-0.1.0-esp-idf-source.tar.gz`: ESP-IDF v4.4.7, commit 38eeba213aa695aabfd6d89aa9f5078dbe5a94c3, with pinned recursive submodules and their notices (Git metadata excluded).

## Rebuild

Install Python 3 and PlatformIO 6.2.0 in a virtual environment, then run `pio run -d projects/pocket-radio` from the project source. platformio.ini pins the platform and top-level libraries. PlatformIO retrieves its compiler/tool packages. To use the archived library sources, extract dependency-inputs and copy the three library directories into the project's `lib/` directory; keep their contents and notices intact. The included framework and platform directories preserve the build inputs used by this release and may be installed into a separate PlatformIO core directory for inspection/rebuilding.

No signing secret or device credential is required to build or install a modified firmware. The StickS3 accepts replacement firmware through its USB bootloader. A full factory installation clears settings. The release's merged image uses offset 0x0; an application-only update uses 0x10000 with the matching partition layout. The package script documents all segment offsets and blank-NVS verification.

Original Pocket Radio code is GPL-3.0-or-later. Dependency files retain their own notices; consult THIRD_PARTY_NOTICES.md and the license files within the archives. SDK vendor binary components are preserved as supplied by Espressif; the ESP-IDF archive is the upstream 4.4.7 source, not a claim of bit-for-bit SDK reproduction. Build output may vary with environment and paths.
