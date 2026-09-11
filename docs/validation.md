# Validation — 2026-09-09

## Research and network

- Retrieved official M5Stack platform, upload, power and speaker documentation.
- Inspected M5Unified's upstream audio example and actual library API headers.
- Retrieved all three broadcaster playlists; configured endpoints match them.
- TLS inspection of ice2.somafm.com showed a valid wildcard certificate issued by ZeroSSL RSA DV SSL CA 2. Selected USERTrust RSA Certification Authority as the trust anchor.
- Live stream probes did not return playable bytes in this environment (timeouts/empty responses). Presets are broadcaster-listed, **not listening-verified**. Test on the target Wi-Fi before release; if a host stays unavailable, try ice5 or ice6 from the official playlist and check its certificate chain.

## Hardware

An Espressif USB JTAG/serial device was enumerated. It was not identified by flashing or modified. No on-device playback, display, controls, power or reconnection tests have been performed. Use the checklist in the radio README.

## Build

Both PlatformIO builds passed with no compiler warnings:

- No credentials header: setup-message firmware, 535,305 bytes flash and 26,320 bytes static RAM.
- Placeholder credentials header: full playback firmware, 1,207,941 bytes flash (36.1% of app partition) and 85,808 bytes static RAM (26.2%). Dynamic TLS/decoder allocations are additional.

Environment: PlatformIO Core 6.2.0, espressif32 6.12.0, M5Unified 0.2.21, M5GFX 0.2.28, ESP8266Audio 1.9.7. The temporary placeholder credentials header was removed after validation. Rebuild with your real local header before uploading. Compilation is not a hardware acceptance test.

## First device listening report

The user confirmed playback after uploading with local Wi-Fi settings, and reported low volume. Raised startup volume from 80/255 to 128/255 and the cap from 160/255 to 184/255, retaining the vendor recommendation to stay below 75% on battery. Loudness/distortion at the new settings still needs user feedback. The full acceptance checklist remains outstanding.

## Motion color and battery update

Firmware build passed (1,222,009 bytes flash; 86,264 bytes static RAM). Host gesture tests passed for rest/orientation, sustained acceleration, two peaks, timeout, cooldown, invalid samples and clock rollover. Battery and charging APIs were checked against the pinned M5Unified StickS3 implementation; percentage is a voltage estimate. On-device gesture sensitivity, layout, persistence and charging indication remain to be checked.

## Curated stations and BBC transport

Tiki Time, KEXP, NTS 1, KCRW Eclectic 24 and Radio Paradise each returned MP3-labeled bytes during host probes. BBC TS extraction passed a real-segment test: decoded PCM hash matched the original segment. Synthetic tests passed for PES extraction, small output buffer, truncated TS, invalid sync, scrambling, invalid adaptation length and continuity errors. Device-side HLS playback and switching remain unverified.

## BBC playback repair

Found ESP8266Audio 1.9.7's undersized SBR output buffer: it allocated 1024 samples per channel, but HE-AAC needs 2048. `RadioAAC` reserves the full stereo buffer and checks allocation before beginning. This applies the same sizing correction as upstream PR #807 while retaining Arduino 2.x compatibility. Also implemented a truly nonblocking HLS read for compressed-buffer prefetch, avoiding multi-second stalls at segment boundaries.

Build/upload passed. Device initially remained in download mode; a watchdog reset through esptool started the application. Hardware logs then showed HTTP 200 for BBC manifests/segments, ~77 KB extracted AAC per segment, increasing output frame counts, and nonzero PCM peaks without AAC decode errors. Switching away to KEXP and NTS also produced decoded audio. These are actual device diagnostics; audible quality still requires the user's confirmation.

USB diagnostics: `b` selects BBC, `t` selects Tiki Time, `?` reports station/status, decoded frame count, peak amplitude and free heap/PSRAM. No credentials are printed. Upstream reference: https://github.com/earlephilhower/ESP8266Audio/pull/807

## Current lineup: BBC replaced

At the user's request, replaced BBC Radio 1 with Illinois Street Lounge (display: Illinois St Lounge), a vintage lounge/exotica station that complements Tiki Time. Official playlist: https://somafm.com/illstreet.pls. The selected ice2 HTTPS endpoint returned HTTP 200 and MP3 audio bytes. Removed the HLS transport, TS parser and AAC wrapper from the project; all six presets now use the common MP3 playback path. Earlier BBC notes above are historical.

Illinois Street Lounge on-device check: HTTP 200, nonzero PCM peaks, and output frame counts advancing at approximately 44.4k stereo frames/second across three successive five-second observations. Some MP3 synchronization errors occurred and recovered; this is not a claim of artifact-free playback. Rate-limited decoder diagnostics to avoid USB log flooding interfering with audio.
# Wi-Fi setup implementation — September 11, 2026

- PlatformIO build passed for StickS3 with the new WebServer/DNSServer portal: 1,272,457 bytes flash, 87,440 bytes static RAM. Build log: `/tmp/sticks3-wifi-build.log` (local temporary artifact).
- Host portal input tests passed: SSID byte limits including UTF-8, password lengths and hex PSKs, open networks, embedded NUL rejection, and HTML escaping. Existing shake tests also passed.
- Source no longer includes `secrets.h` or uses `RADIO_WIFI_*` macros.
- No StickS3 USB serial port was available; no firmware was uploaded. AP startup, phone captive-page behavior, channel changes during association, saved-settings persistence, failed-password recovery, setup gesture, and return to audio remain unverified on hardware. Follow the updated project README acceptance checklist.

## Wi-Fi setup upload — September 11, 2026

Uploaded the setup firmware to `/dev/cu.usbmodem101` after the user entered upload mode. PlatformIO succeeded and esptool verified the written data hash; log: `/tmp/sticks3-wifi-upload.log`. No serial boot output was captured afterward (the portal loop has no serial status handler), so on-screen startup and phone provisioning are still awaiting user verification.

## Station selection and visible Wi-Fi picker — September 11, 2026

User confirmed the initial phone provisioning flow worked on the device. Added a persisted subset of the six presets, dial filtering, a Return to radio action using saved Wi-Fi, and a visible network suggestion list with automatic scan polling, filtering, deduplication, signal labels, refresh, and manual-name fallback. Existing Wi-Fi credential record format is unchanged.

PlatformIO build passed (`/tmp/sticks3-station-picker-build.log`). Host tests passed for subset traversal/wraparound, one-station selection, invalid-mask rejection, credential validation, and HTML/JSON escaping. Playwright mobile-width checks passed using actual embedded page assets with simulated network/storage responses; inspected `/tmp/sticks3-setup-page.png`. No horizontal overflow or browser exceptions were observed. These checks do not verify the ESP32 endpoints or NVS.

No USB StickS3 was present after the build; this update has not been flashed. On-device checks still needed: existing Wi-Fi reconnect after upgrade; real scan/refresh on phone; station save/reboot persistence; a single-station dial; disabled current station falling back to the first selected station; Return to radio without re-entering credentials; and unchanged color/volume controls.

### Station picker upload

Uploaded to `/dev/cu.usbmodem101` at the user's request. PlatformIO succeeded and esptool verified the written data hash. Log: `/tmp/sticks3-station-picker-upload.log`. Phone interaction, selection persistence, and playback after this update still await hardware acceptance.

## Wi-Fi first and optional stations — September 11, 2026

Moved Wi-Fi above station selection; appended four SomaFM choices with a default mask selecting only the original six. Existing saved selections are preserved. Firmware build and station-selection host tests passed. Mobile-browser checks passed for section order, ten choices, six initial checkmarks, filtering and save behavior; screenshot inspected. Four host stream probes returned MP3 data with verified TLS; on-device listening remains pending. Uploaded to `/dev/cu.usbmodem101` successfully with flash hash verification. Log: `/tmp/sticks3-more-stations-upload.log`.

## Release candidate 0.1.0-rc1

Built from an isolated allowlisted source copy with no secrets header. Generated an 8,388,608-byte factory image at offset 0x0 and app-only image at offset 0x10000. Verified the NVS region is entirely 0xff, merged partition/OTA/app segments match inputs, archive excludes secrets and .pio outputs, and all artifact SHA256SUMS match. Three host C++ suites passed. Package: `dist/pocket-radio-0.1.0-rc1/`; source manifest identifies the exact packaged inputs. Later documentation edits do not change that archived snapshot.

Uploaded only the candidate app to preserve existing settings; esptool verified its hash (`/tmp/sticks3-rc1-upload.log`). USB diagnostics reported version 0.1.0-rc1, Tiki Time, LIVE / INTERNET RADIO, increasing decoded frame counts, and nonzero peaks. This establishes restart/reconnection and decoder output on the existing device; it is not an audible quality or extended stability test.

Clock failure messaging/retry and setup diagnostics are implemented but clock-failure injection remains pending. Factory-image installation through M5Burner, full candidate hardware acceptance, licensing/corresponding source, public repository, and store publication remain pending.
