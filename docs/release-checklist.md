# Pocket Radio — first M5Burner release checklist

Updated September 11, 2026. Phone setup and station selection are implemented. Initial provisioning was confirmed by the user; candidate-specific hardware acceptance remains pending. The local 0.1.0-rc1 candidate adds version diagnostics, clearer clock-sync failure/retry behavior, clean packaging, and a draft store listing. See [hardware acceptance](release-acceptance.md) and [license review](license-review.md).

## 1. Make Wi-Fi setup work for everyone — main blocker

- [x] Choose the first-run setup flow. Implemented: the radio creates a temporary Wi-Fi network and serves a phone-friendly setup page; display connection instructions and a fallback local address on screen.
- [x] Implement device-stored Wi-Fi settings and automatic connection without compiling. Full reboot/recovery acceptance is tracked below.
- [x] Implement A+B setup access, a saved-network return action, and Forget saved Wi-Fi on the setup page.
- [ ] Handle wrong passwords, unavailable networks, and failed clock sync with clear messages and a way back to setup.
- [ ] Test setup on a phone and laptop, including reboot, reconnect, and credential reset. Document 2.4 GHz Wi-Fi and unsupported captive-portal/enterprise networks.

## 2. Settle the station experience

- [x] Establish the default dial: Tiki Time, Illinois Street Lounge, KEXP, NTS 1, KCRW Eclectic 24, and Radio Paradise. Keep Tiki Time first; BBC, WWOZ, and FIP Groove remain excluded. Optional extras: Bossa Beyond, Secret Agent, Suburbs of Goa, Heavyweight Reggae.
- [x] Implement browser selection from the curated catalog. Custom URLs and reordering remain follow-up features.
- [ ] Verify every chosen stream on the StickS3: audible playback, correct speed, stable switching, redirects, and certificate validation. Record results and dates in validation notes.
- [ ] Ensure an unavailable station shows a useful status and users can move on without getting stuck in retries.
- [ ] Link to broadcasters and identify any regional restrictions. Define how broken presets will be reported and updated; custom URLs, if supported, need documented format and TLS limitations.

## 3. Validate everyday use

- [ ] Check all button taps/holds, stop/resume, shake colors, saved color, and battery/charging display on the release candidate.
- [ ] Verify power off and restart; clearly document double-clicking the side power/reset button to shut down. A long press enters upload mode; stopping playback is not shutdown.
- [ ] Test Wi-Fi loss/restoration, unreachable streams, and repeated station switching; fix crashes, persistent stalls, or unresponsive controls.
- [ ] Complete an extended listening session on USB and battery, including the maximum allowed volume; record dropouts, runtime, and resets. Retain the conservative battery volume cap.
- [ ] Have another person complete a fresh install and first-run setup using only the published instructions.

## 4. Prepare source and release files

- [ ] Choose the project license after reviewing the pinned dependencies and bundled codecs; retain upstream notices and include the appropriate corresponding source and build instructions.
- [ ] Prepare the public GitHub repository with a quick start, controls, station list, supported hardware (StickS3), known limitations, credits, and issue-reporting instructions.
- [ ] Choose the release name/version and tag the exact source used for the firmware. Keep dependency versions pinned and document a repeatable release build.
- [ ] Build public firmware without personal credentials. Check source, repository history, release files, and logs for secrets without exposing them in reports.
- [ ] Package a complete image suitable for M5Burner, with the correct flash layout and offsets; confirm the required format in the current publishing UI. Do not assume the application-only binary is sufficient.
- [ ] Ensure the package contains no personal device settings/NVS. If using M5Burner's export workflow, export only a deliberately clean release device, never the configured everyday radio.
- [ ] Test the exact packaged image through M5Burner from a clean device state, then verify setup, all presets, and reboot. Record its checksum and matching source version.

## 5. Prepare and publish the M5Burner listing

- [ ] Sign in with an M5Stack community account.
- [ ] Prepare the firmware name, version, description/instructions, StickS3 device selection, GitHub link, firmware image, and cover image.
- [ ] Include an actual device photo or screenshot, Wi-Fi setup instructions, controls (especially power off), and known limitations in the release materials.
- [ ] Upload through USER CUSTOM → Publish, review the listing, and set its publication status when the release is ready.
- [ ] Verify the public listing and download, test installation from that download, and save the share code/link.
- [ ] Keep the GitHub release and M5Burner version aligned; provide a place for station suggestions and bug reports.

M5Stack documents the account requirement, publishing fields, upload workflow, publication status, and share code in its [M5Burner publishing guide](https://docs.m5stack.com/en/uiflow/m5burner/publish), checked September 11, 2026. The other checks above are our project release criteria, not additional store requirements. Power-button behavior is documented in the [StickS3 guide](https://docs.m5stack.com/en/core/StickS3#button-operation-instructions).

## Optional after the first release

- [ ] Browser-based custom stations, favorites, and reordering, if deferred above.
- [ ] Remember the last station and volume with a sensible startup volume limit.
- [ ] Track metadata, more audio formats, and additional device ports.
- [ ] External audio output support once hardware is selected and tested.

Suggested order: decide Wi-Fi and stations → implement onboarding → hardware testing → source and firmware packaging → M5Burner listing and publication.

## Prepared release materials

- [x] Add version 0.1.0-rc1 to portal and USB diagnostics.
- [x] Add isolated release build/packaging script with blank-NVS verification, source hashes, and checksums.
- [x] Draft the [M5Burner listing](../projects/pocket-radio/release/m5burner-listing.md).
- [x] Prepare a [candidate hardware test record](release-acceptance.md).
- [ ] Finalize license and GitHub account/repository (user preferences requested).
- [ ] Test the candidate factory image in M5Burner; complete device acceptance and source/license preparation before publication.
