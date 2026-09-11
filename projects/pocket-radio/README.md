# Pocket Radio

A small StickS3 internet radio. Ten available live stations, with the original six selected by default, a monochrome color dial, play/stop, bounded volume and automatic retries. MP3 playback has been heard on hardware; new presets still need listening validation.

## Setup

From the workspace root:

```sh
python3 -m venv .venv
.venv/bin/pip install platformio==6.2.0
```

Wi-Fi is configured from a phone after flashing; `secrets.h` is no longer included in the firmware. Existing private headers can remain locally, but are not used.

On first boot:

1. Join the `PocketRadio-XXXX` Wi-Fi network shown on the radio, using the temporary password displayed underneath.
2. Stay connected if your phone reports no internet. A setup page may open automatically; otherwise open **http://192.168.4.1/** in your browser (HTTP, not HTTPS).
3. Tap a nearby 2.4 GHz network in the visible list, or type to filter it. The list loads automatically after scanning; Refresh starts a new scan. Hidden networks can be entered manually. Enter the network password.
4. Press **Connect radio**. The radio tests the connection for up to 25 seconds, saves successful settings on-device, and closes setup after five more seconds. Return your phone to its usual Wi-Fi.

The radio then synchronizes its clock and starts streaming. Wi-Fi connection success does not guarantee internet access; captive-portal networks are unsupported. A failed connection leaves setup available and preserves the previously saved network. Phone disconnection during the attempt is possible when the radio changes Wi-Fi channels; rejoin its setup network to retry if needed.

Hold **A+B together for 1.5 seconds** to reopen setup. It also opens after 45 seconds of failed Wi-Fi connection while playback is enabled. Use **Forget saved Wi-Fi** on the page to erase the saved network. Reset the radio to leave setup and retry a previously saved network. The temporary network password changes each time setup opens.

The page and its styles/scripts live in `include/SetupPage.h`, with routes in `include/WifiSetup.h` and are served directly by the device; there are no external web assets or accounts. Credentials are stored in the device's NVS, not in the public firmware. Do not publish a flash dump from a configured device.

The initial phone setup flow has been confirmed working by the user. The new station picker and visible Wi-Fi suggestions pass host/browser checks but still need phone and device acceptance testing.

```sh
.venv/bin/pio run -d projects/pocket-radio
.venv/bin/pio device list
.venv/bin/pio run -d projects/pocket-radio -t upload --upload-port /dev/cu.YOUR_DEVICE
.venv/bin/pio device monitor -b 115200 -p /dev/cu.YOUR_DEVICE
```

Uploading replaces the current firmware. Substitute the actual port. If upload cannot connect, follow [M5Stack's download-mode instructions](https://docs.m5stack.com/en/core/StickS3): connect USB and hold the side reset button until the internal green LED flashes.

## Choose your stations

Hold A+B for 1.5 seconds, join the setup network shown on the screen, and open http://192.168.4.1/. Check the stations you want under **Your stations**, then press **Save stations**. At least one must remain selected. Press **Return to radio** to reconnect using the saved Wi-Fi password. Wi-Fi appears first on the page. On first setup you can use the default six immediately, or scroll down and save station choices before connecting.

The dial skips unchecked stations and remembers the selection across restarts. Choices currently come from ten bundled presets; custom stream URLs and reordering are not implemented. Forgetting Wi-Fi does not erase station preferences or color.

## Controls

| Input | Action |
| --- | --- |
| A tap | Next preset, wrapping to the beginning |
| A hold (0.6 seconds) | Stop/resume the live stream |
| B tap | Volume up |
| B hold (0.6 seconds) | Volume down |
| Deliberate back-and-forth shake | Next monochrome color |
| A+B hold (1.5 seconds) | Open Wi-Fi setup |
| Side power/reset double-click | Power off |
| Side power/reset single-click | Power on / reset |

Stopping closes the stream; resuming rejoins the live broadcast. The radio starts on the first selected station at the default volume on reboot; the color is remembered. Initial volume is 50%, maximum 72%. The power/reset button is separate from A and B.

## Stations and limitations

Edit `include/stations.h` to change presets. Use short display names and direct HTTPS MP3 URLs. New broadcasters may require an additional CA root in `include/root_ca.h`. Current dial: Tiki Time, Illinois Street Lounge, KEXP, NTS 1, KCRW Eclectic 24 and Radio Paradise. Streams connect directly to each broadcaster. Support the stations you enjoy.

Wi-Fi and NTP must work before playback. Captive portals and enterprise Wi-Fi are not supported. The status line reports connection, clock sync and retry states. Failed streams retry after ten seconds; Wi-Fi reconnects every fifteen seconds. Network calls can briefly delay input. There is no track metadata or favorites storage. Direct MP3 responses still cannot use chunked transfer encoding. All presets now use direct MP3 streams.

## Hardware acceptance checklist

- Boot without saved Wi-Fi: setup network and readable on-screen instructions.
- Join from a phone; verify captive-page detection and the manual HTTP address.
- Try incorrect credentials, retry correctly, reboot, and verify remembered Wi-Fi.
- Reopen setup with A+B, change networks, and forget saved settings.
- Check HTML-sensitive network names, hidden networks entered manually, and open networks.
- Configure Wi-Fi: connects, synchronizes time, plays audible music.
- Try all six presets; confirm names fit and pitch/speed sound right.
- Check each tap and hold triggers only its intended action.
- Stop and resume repeatedly; no stale audio or crashes.
- Disconnect the access point, restore it, and confirm recovery.
- Try an invalid stream URL; verify retry status and station switching.
- Listen for 20 minutes on USB, then battery; note dropouts or resets.

See `../../docs/platform-notes.md` for sources and design decisions. The speaker adapter is based on M5Stack's MIT example; see `M5STACK-LICENSE`. ESP8266Audio has GPL obligations for distributed firmware; preserve notices and provide corresponding source.

## Color and battery

Shake the unit back and forth to cycle yellow → cyan → green → magenta → orange → white. Two acceleration peaks within 700 ms are required, followed by a 1.5-second cooldown. Rotating slowly or a single tap should not change color. Motion input can be delayed during network connection/stalls.

The header shows `BAT ~75%` as an approximate voltage-based battery estimate; `+` means charging. It refreshes every ten seconds. `BAT --` means the reading is unavailable. The estimate can vary with speaker load and charging.

To run the gesture logic checks from the workspace root:

```sh
c++ -std=c++11 -I projects/pocket-radio/include projects/pocket-radio/tests/shake_test.cpp -o /tmp/sticks3-shake-test
/tmp/sticks3-shake-test
```

On hardware, verify ordinary handling does not trigger colors, a shake advances only once, the choice survives restart, all colors remain legible, and the charging marker changes after unplugging USB. Gesture sensitivity still needs real-world feedback.


## USB diagnostics

At 115200 baud, send `t` to select Tiki Time, `l` for Illinois Street Lounge, or `?` for playback statistics. BBC and its HLS/AAC implementation have been removed.

## Setup input checks

```sh
c++ -std=c++11 -Wall -Wextra -I projects/pocket-radio/include projects/pocket-radio/tests/portal_input_test.cpp -o /tmp/sticks3-portal-test
/tmp/sticks3-portal-test
```

These cover credential byte limits, embedded NUL rejection, open networks, hexadecimal PSKs, and escaping network names for HTML. They do not replace on-device provisioning and storage tests.

Station-selection checks:

```sh
c++ -std=c++11 -Wall -Wextra -I projects/pocket-radio/include projects/pocket-radio/tests/station_selection_test.cpp -o /tmp/sticks3-station-test
/tmp/sticks3-station-test
```

With Playwright and Chromium installed, run `node projects/pocket-radio/tests/setup_page_test.cjs` from the workspace root (set `NODE_PATH` if using a shared installation). This exercises the embedded page assets with simulated scan/save endpoints: filtering, touch/click selection, keyboard selection, refresh, manual entry, empty-station prevention, saving without clearing entered Wi-Fi details, and mobile-width layout. Actual ESP32 scanning, persistence, and playback still need device tests.

## Extra station choices

The original six are checked on a fresh device. Four optional stations start unchecked under **More to explore**: Bossa Beyond (bossa nova/samba), Secret Agent (cinematic lounge), Suburbs of Goa (Desi/global beats), and Heavyweight Reggae (reggae/ska/rocksteady). Saved selections remain unchanged when updating firmware. New presets were appended to preserve existing saved selection bits. All four returned HTTP 200 and MP3 data in host checks; listening on StickS3 is still pending.

## Release candidates

Version `0.1.0-rc1` appears in the setup footer and USB `?` diagnostics (also available during setup). If clock sync has not completed after 30 seconds, the status changes to **CLOCK FAILED / CHECK WI-FI** and retries every 30 seconds; hold A+B to return to setup.

See [release packaging](release/README.md) for the isolated build and clean factory-image workflow. Factory images reset settings; application-only upgrades preserve the existing matching layout and settings. Publication and full hardware acceptance are still pending.
