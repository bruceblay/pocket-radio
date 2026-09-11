# Broader station options — 2026-09-09

Read the current built-in presets from https://vvorldwide.com/ (downloaded HTML; no site code imported into the firmware). These are candidate broadcaster endpoints for a more varied lineup. No firmware preset changes have been made yet.

| Station | Programming | VVORLDWIDE-listed direct endpoint |
| --- | --- | --- |
| KEXP | DJ-led music discovery, alternative and eclectic | https://kexp-mp3-128.streamguys1.com/kexp128.mp3 |
| NTS 1 | Global DJ shows and eclectic selections | https://stream-relay-geo.ntslive.net/stream |
| WWOZ | New Orleans jazz and heritage | https://wwoz-sc.streamguys1.com/wwoz-hi.mp3 |
| KCRW Eclectic 24 | Music-only eclectic mix | https://streams.kcrw.com/e24_mp3 |
| Radio Paradise | Eclectic mix | https://stream.radioparadise.com/mp3-192 |
| FIP Groove | Funk, soul and hip-hop | https://icecast.radiofrance.fr/fipgroove-midfi.mp3 |

All six returned HTTP 200, audio/mpeg and 1,024 bytes in a short host-side probe with normal TLS verification. This confirms MP3-labeled responses, not decoded playback or on-device compatibility. FIP Groove uses chunked transfer encoding: the current RadioSource explicitly rejects that, so it needs a transport enhancement. The other five returned no Transfer-Encoding header. The device's single embedded CA root must be checked/expanded for the selected broadcasters before uploading. Do not bypass TLS validation.

Additional VVORLDWIDE options: NTS 2, dublab, Refuge Worldwide, Worldwide FM, FIP Rock, FIP Jazz, Nightwave Plaza and NASA Third Rock Radio. BBC Radio 1/2/6 Music entries use HLS and need a larger playback change. Those additional endpoints have not been probed.

Recommended first replacement lineup: KEXP, NTS 1, WWOZ, KCRW Eclectic 24, Radio Paradise. Add FIP Groove after chunked-response support. Keep a local curated list initially; fetching the site's HTML on-device would couple firmware to an undocumented page layout. A future station-management feature could import a small explicit JSON list.

## Selected lineup

User excluded WWOZ and FIP Groove and requested Tiki Time and BBC Radio 1. Implemented dial: Tiki Time, BBC Radio 1, KEXP, NTS 1, KCRW Eclectic 24, Radio Paradise. Tiki Time returned audio/mpeg bytes. NTS uses a verified HTTPS redirect to a regional Radiomast host; firmware now supports up to three absolute HTTPS redirects. Trust anchors were obtained from system-verified TLS chains for each host, including NTS's regional host.

BBC master: https://a.files.bbci.co.uk/ms6/live/3441A116-B12E-4D2F-ACA8-C1984642FA4B/audio/simulcast/hls/nonuk/pc_hd_abr_v2/ak/bbc_radio_one.m3u8

Its current international variant is 96 kbps HE-AAC in MPEG-TS, available over HTTPS. A real 81,592-byte segment was fetched; firmware's extraction routine produced AAC that ffprobe identified as HE-AAC, stereo, 48 kHz. Decoding the original TS and extracted AAC with ffmpeg produced identical SHA-256 hashes of PCM output. The device-side decoder and prefetch timing still need a listening test.

## Current lineup: BBC replaced

At the user's request, replaced BBC Radio 1 with Illinois Street Lounge (display: Illinois St Lounge), a vintage lounge/exotica station that complements Tiki Time. Official playlist: https://somafm.com/illstreet.pls. The selected ice2 HTTPS endpoint returned HTTP 200 and MP3 audio bytes. Removed the HLS transport, TS parser and AAC wrapper from the project; all six presets now use the common MP3 playback path. Earlier BBC notes above are historical.

## Optional catalog additions — September 11, 2026

Added four SomaFM streams below the original six. Defaults remain the first six only; existing saved selections are preserved.

- [Bossa Beyond](https://somafm.com/bossa/): bossa nova and samba; `https://ice2.somafm.com/bossa-128-mp3`.
- [Secret Agent](https://somafm.com/secretagent/): cinematic downtempo and lounge; `https://ice2.somafm.com/secretagent-128-mp3`.
- [Suburbs of Goa](https://somafm.com/suburbsofgoa/): Desi-influenced global beats; `https://ice2.somafm.com/suburbsofgoa-128-mp3`.
- [Heavyweight Reggae](https://somafm.com/reggae/): reggae, ska and rocksteady; `https://ice2.somafm.com/reggae-128-mp3`.

SomaFM provides direct MP3 links for hardware players on each channel's directstreamlinks.html page. Direct server addresses can change. Host probes returned HTTP 200, audio/mpeg, no Transfer-Encoding, and 8,192 sample bytes for all four. TLS verified with Python's TLS client using the firmware CA bundle; this does not establish ESP32 TLS or decoder compatibility. On-device listening is pending.
