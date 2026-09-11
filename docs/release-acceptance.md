# Pocket Radio 0.1.0-rc1 — hardware acceptance

Record results against the candidate's SHA256SUMS. A successful compile or upload is not a pass for playback or phone interaction. Status: not yet completed.

## Quick pass on the existing device (preserve settings)

- [x] Start the candidate; saved Wi-Fi reconnects and clock sync finishes. Serial confirms live Tiki Time decoding with nonzero output; audible quality remains to be checked.
- [ ] Hold A+B; join setup from a phone. Confirm the footer version is 0.1.0-rc1.
- [ ] Wi-Fi appears before station selection. Nearby results load automatically, filter when typing, and refresh.
- [ ] Select only Tiki Time and Secret Agent, save, then Return to radio without entering the Wi-Fi password. A cycles only those two.
- [ ] Power off with a double-click of the side power/reset button; single-click to power on. Selection and color persist.
- [ ] Try an incorrect password in setup. After timeout, correct it. The previous saved network must remain available until the successful replacement.
- [ ] With Wi-Fi unavailable for 45 seconds during playback, setup opens; reconnect to a working network and resume.
- [ ] With clock synchronization blocked, the radio reports failure after 30 seconds, continues retrying, and A+B still opens setup.
- [ ] Every individual tap/hold, stop/resume, volume control, shake, and charging marker works as documented.

## Stations

Enable all ten temporarily, listen to each, and restore the preferred dial afterward. Record date, USB/battery, audible playback, switching, and any dropouts.

| Station | Result / observations |
| --- | --- |
| Tiki Time | Pending on this candidate |
| Illinois Street Lounge | Pending on this candidate |
| KEXP | Pending on this candidate |
| NTS 1 | Pending on this candidate |
| KCRW Eclectic 24 | Pending on this candidate |
| Radio Paradise | Pending on this candidate |
| Bossa Beyond | Host connection passed; device listening pending |
| Secret Agent | Host connection passed; device listening pending |
| Suburbs of Goa | Host connection passed; device listening pending |
| Heavyweight Reggae | Host connection passed; device listening pending |

- [ ] At least 30 minutes on USB and a battery listening session; record duration, volume, dropouts, and resets.
- [ ] Move off an unavailable station and recover without rebooting.

## Clean installation through M5Burner

The factory image deliberately replaces the entire 8 MB flash, including saved settings. Schedule this test when the owner is ready to reconfigure, or use a spare StickS3.

- [ ] Confirm current Burner accepts the raw factory image and uses offset 0x0 / StickS3.
- [ ] Install the exact candidate factory image through Burner.
- [ ] First boot presents setup and has no previous credentials, station preferences, or color.
- [ ] The original six stations start checked; the four extras start unchecked.
- [ ] Complete phone setup using only the quick-start instructions; reboot and verify saved settings.
- [ ] Forget Wi-Fi from setup, reboot, and confirm first-run setup returns.
- [ ] Repeat setup on a laptop and have another person follow the instructions without assistance.

Do not mark store publishing or clean-install acceptance complete until these have actually been exercised.
