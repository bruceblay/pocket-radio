# Pocket Radio release candidate

Local test candidate, not an approved public release. Version is declared in `include/Version.h` and shown in the setup page footer and USB `?` diagnostics.

Run from the workspace root:

```sh
.venv/bin/python projects/pocket-radio/tools/package_release.py
```

This builds from a temporary allowlisted source copy without `secrets.h`, merges build outputs, and writes `dist/pocket-radio-<version>/`. It never reads device flash or uploads. An existing output directory causes an error; use a new version for a new candidate or deliberately move an obsolete local candidate aside.

## Files

- `*-factory.bin`: complete 8 MB image for offset **0x0**, with blank NVS verified. Installing it resets saved Wi-Fi, station choices and color. It also clears the other application/data areas. Do not use for a routine upgrade that should preserve settings.
- `*-app.bin`: application only, offset **0x10000**, for an existing matching flash layout. This is not a complete first-install image.
- `*-source.tar.gz`: the project's exact allowlisted build inputs. Third-party dependency source is not bundled; this is not yet the complete corresponding-source distribution.
- `manifest.json`: version, flash segments, source hashes, and remaining release gates.
- `SHA256SUMS`: artifact checksums. `build.log` is local diagnostic output and is not intended for publication.
- `dependency-notices/`: copied dependency notices and version metadata. This collection is preliminary, not a completed license audit.

Build settings: PlatformIO 6.2.0, espressif32 6.12.0, Arduino ESP32 2.0.17, ESP32-S3 with 8 MB flash and OPI PSRAM. Offsets were checked against PlatformIO envdump: bootloader 0x0, partitions 0x8000, OTA initialization 0xe000, application 0x10000. Factory image flash mode is DIO, 80 MHz; verify against the installed platform before changing the build configuration.

## Before publication

1. Finalize licensing and prepare corresponding source and notices for distributed dependencies.
2. Create the public repository and tag the exact source version.
3. Complete the hardware acceptance record, including all ten presets and recovery tests.
4. Confirm the current M5Burner uploader's file format, select StickS3, and test this exact image through M5Burner from a clean device state. The raw merged image is a candidate until that test passes.
5. Supply a real device photo/cover and the finalized repository URL in the listing.

[M5Stack publishing guide](https://docs.m5stack.com/en/uiflow/m5burner/publish) and [export guide](https://docs.m5stack.com/en/uiflow/m5burner/export). Build outputs are used instead of a dump of the owner's configured device.
