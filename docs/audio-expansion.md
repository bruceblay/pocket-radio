# Louder audio options — 2026-09-09

User likes Tiki Time and wants substantially louder playback. No hardware purchased or firmware changed for external audio.

## Recommended paths

1. **Line out into powered speakers:** [Adafruit PCM5102 I2S DAC](https://www.adafruit.com/product/6250), listed at $4.95 during research, provides a stereo 3.5mm line output. Needs three digital audio signals (BCLK, WSEL, DIN), power and ground, plus firmware output routing. No MCLK or I2C setup needed. Its jack is line-level, not a headphone amplifier; use powered speakers or an amplifier's AUX input. Wiring/pin selection on StickS3 must be verified before assembly.
2. **Build a larger self-contained speaker:** [Adafruit MAX98357A I2S amplifier](https://learn.adafruit.com/adafruit-max98357-i2s-class-d-mono-amp?view=all), listed at $5.95, plus a larger 4-ohm speaker rated for 3W and an enclosure. Advertised 3.2W is at 5V into 4 ohms with 10% THD, not a promise of clean output at that level. Requires three audio signals, power and ground. Design a suitable external 5V supply; do not assume the StickS3's limited expansion power can provide full amplifier output. Bridge-tied speaker outputs must not be connected to an AUX input or grounded.
3. **Speaker hat:** [M5Stack Hat SPK2 U055-B](https://docs.m5stack.com/en/hat/Hat-SPK2) uses MAX98357 I2S and includes a 1W speaker. Documentation targets StickC-Plus and does not establish StickS3 plug-and-play compatibility. Confirm connector orientation, pin mapping and physical fit before buying for this device. Its amplifier's 3.2W specification does not make the included speaker a 3.2W driver, nor prove it will be substantially louder than StickS3. The [older Hat SPK U055](https://docs.m5stack.com/en/hat/hat-spk) is an analog-input PAM8303 design; do not confuse these models.

[StickS3 hardware reference](https://docs.m5stack.com/en/core/StickS3) lists an onboard 1W speaker and a limited external 5V output. External I2S would be routed to selected expansion GPIOs; no internal speaker-wire modification is needed for either proposed DAC or amplifier approach. Grove alone has only two signal pins, so these three-signal interfaces need the expansion header or another signal connection. A final wiring design has not been produced or tested.

[Espressif Bluetooth documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/api-guides/bt-architecture/overview.html): ESP32-S3 has BLE, but no Bluetooth Classic. Ordinary A2DP Bluetooth speaker output is not a firmware-only option on this chip.

Recommendation: PCM5102 plus an existing AUX-equipped powered speaker for the greatest practical improvement; MAX98357A plus a larger enclosed speaker for a custom portable Tiki-radio build. Both need firmware work. Product prices exclude shipping/tax and can change.
