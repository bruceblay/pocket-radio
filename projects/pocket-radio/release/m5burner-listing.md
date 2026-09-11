# M5Burner listing draft

Name: **Pocket Radio**

Device: **StickS3**

Candidate version: **0.1.0-rc1**

GitHub: https://github.com/bruceblay/pocket-radio

Cover: Pending real device image.

## Description

A little internet radio for your M5Stack StickS3. Tune into live stations, build your own dial from ten curated choices, and shake the radio to change its monochrome color.

Six stations are selected to start: Tiki Time, Illinois Street Lounge, KEXP, NTS 1, KCRW Eclectic 24 and Radio Paradise. Add Bossa Beyond, Secret Agent, Suburbs of Goa or Heavyweight Reggae from the setup page.

### First-time setup

1. Join the PocketRadio Wi-Fi network shown on the radio, using its displayed password.
2. Open **http://192.168.4.1/** on your phone if the setup page doesn't appear automatically.
3. Choose your home 2.4 GHz Wi-Fi, enter its password, and tap **Connect & play**.

The radio remembers your network. No phone app, account or code editing is required.

### Controls

- A tap: next selected station. A hold: stop/resume.
- B tap: volume up. B hold: volume down.
- Shake: change color.
- Hold A+B together for 1.5 seconds: open Wi-Fi and station setup.
- Double-click the separate side power/reset button: power off. Single-click: power on/reset. Long press: upload mode.

To change your dial, check your stations and tap **Save stations**, then **Return to radio**. At least one station must remain selected.

### Notes

For StickS3 only. Uses the built-in speaker; Bluetooth speaker audio is not supported. Requires ordinary 2.4 GHz Wi-Fi with internet access; hotel sign-in pages and enterprise Wi-Fi are unsupported. Battery percentage is approximate. Volume is limited for battery stability. Station availability depends on each broadcaster; network stalls can briefly delay controls. Custom stream URLs and track titles are not supported in this version.

The audio adapter is based on M5Stack's web-radio example. Streams connect directly to the broadcasters; support the stations you enjoy.
