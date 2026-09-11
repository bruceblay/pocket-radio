# Release preparation — September 11, 2026

Version 0.1.0 is prepared locally, not published or tagged. It includes the simplified captive Wi-Fi views approved on the device by Bruce. The latest development upload completed with verified flash hashes.

The isolated package build succeeded. Factory image size is 8 MB, offset 0x0; blank NVS and all packaged checksums passed verification. Files are in ignored `dist/pocket-radio-0.1.0/`. Installing the factory image clears saved settings. No configured device flash was read or exported.

The listing text is in `projects/pocket-radio/release/m5burner-listing.md`; the existing playing-device photo is selected as the cover.

Bruce signed in to the web developer dashboard and approved GPL-3.0-or-later. The license is applied. Pocket Radio 0.1.0 was uploaded successfully as PRIVATE, firmware ID 2098472213560905729, category Audio & Media, device StickS3. Management page: https://burner.m5stack.com/developer/firmware/2098472213560905729/versions

The dashboard offers Burn to device using WebSerial and requires review for public visibility. A request to clear saved settings for the clean-install test is pending. No factory installation was performed.

Dependency inputs are archived locally. ESP-IDF v4.4.7 and its recursive submodules were retrieved successfully (commit 38eeba213aa695aabfd6d89aa9f5078dbe5a94c3). Exact SDK binary provenance and complete source/license review remain open; these archives are not yet described as a completed compliance bundle.

Next: finish applicable dependency source/notices; verify the exact factory image through M5Burner and first-run setup; tag matching source and publish aligned GitHub/M5Burner releases. Outstanding hardware checks remain recorded in release-acceptance.md; do not infer passes from build success.

## M5Burner clean install

Bruce authorized clearing settings and selected the USB port in Chrome. On September 11, 2026 at 13:11 CDT, M5Burner downloaded Pocket Radio 0.1.0, erased the chip, wrote the 8,388,608-byte image at offset 0x0, verified the flash header, reset the device, and reported “Burn successfully.” This confirms the web uploader accepts the raw factory image. The website reports header verification, not a full-image checksum readback. First-run phone provisioning and playback after this installation await user confirmation.

Bruce confirmed fresh phone setup and station playback after the M5Burner installation. Public submission is authorized. Extended recovery and all-station testing remain as recorded in release-acceptance.md.
