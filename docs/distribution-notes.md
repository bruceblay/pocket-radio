# Distribution notes — 2026-09-09

M5Burner is a firmware distribution/flashing catalog, not an exhaustive index of M5Stack projects. Its publishing workflow is separate from pushing code to GitHub. Official guide: https://docs.m5stack.com/en/uiflow/m5burner/publish

Publishing uses an M5Stack community account and USER CUSTOM > Publish. Supply a firmware name, version, description, device type, GitHub link, firmware image and cover image. After uploading, publishing status and a share code can be managed. No publishing action has been taken for Pocket Radio.

Existing examples of radio projects on GitHub include https://github.com/cyberwisk/M5Cardputer_WebRadio and https://github.com/CelliesProjects/eStreamPlayer32 . They target other M5/ESP32 hardware; they do not establish ready-to-flash StickS3 compatibility. M5Stack also maintains its M5Unified web-radio example. These examples demonstrate that radio software exists outside a particular Burner device listing, not what proportion of developers use GitHub versus Burner.

Likely barriers to broad distribution (engineering judgment, not survey evidence): board-specific hardware, user Wi-Fi provisioning, reproducible builds, station URL maintenance, clear instructions and ongoing support. A working personal sketch does not automatically provide these.

For this project, the most concrete release gap is Wi-Fi setup: credentials currently come from a private compiled header. A public build should let each user configure Wi-Fi without compiling, e.g. a phone-accessible setup portal, and should be built cleanly without personal credentials. Also choose a license compatible with dependencies, retain upstream notices, publish corresponding source, document supported hardware and controls, and complete playback/reconnection testing. GitHub source/releases plus a matching M5Burner listing would serve both developers and people who simply want to flash a radio.

## Planning status — September 11, 2026

The actionable plan now lives in the [first-release checklist](release-checklist.md). Wi-Fi onboarding and launch station scope remain the first decisions. No publishing action has been taken.

## Earlier next-session note

The user intends to publish Pocket Radio and wants to resume tomorrow. Defer implementation and publishing for now. First discuss (1) Wi-Fi onboarding without compiling credentials, including changing networks and recovery, and (2) station curation/personalization. Tiki Time is a strong preference; BBC was deliberately removed to favor simpler reliable playback. Resume with design discussion before choosing the setup flow or station-management approach.

## Wi-Fi implementation — September 11, 2026

Phone-based setup is implemented in `WifiSetup.h`, using Arduino's built-in WebServer/DNSServer and Preferences. The current firmware no longer includes `secrets.h`; earlier notes about compiled credentials apply to older binaries. First boot opens a password-protected temporary AP. A+B held for 1.5 seconds reopens setup. A successful connection saves one credential record; failure preserves the old record. Public firmware must still be built cleanly, and configured-device flash exports still contain private NVS settings. Hardware/phone testing is pending because no USB device was present during implementation.
