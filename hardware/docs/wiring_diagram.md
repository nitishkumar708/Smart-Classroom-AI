# Wiring — Final Build

Board: **ESP32 DevKit V1** (30-pin)

## Components

| Component | Qty | Notes |
|---|---|---|
| ESP32 DevKit V1 | 1 | USB powered from the laptop |
| HW-484 sound sensor | 1 | Use the analog output **A0**; power from **3V3 only** |
| Green LED (5 mm) | 3 | |
| Red LED (5 mm) | 2 | |
| 220 Ω resistor | 5 | one per LED |
| Active buzzer | 1 | sounds when the pin is HIGH |
| Breadboard + jumper wires | | |

## Pin assignments

| ESP32 pin | Connects to | Purpose |
|---|---|---|
| GPIO 34 | Sound sensor **A0** | Analog sound input (input-only pin) |
| 3V3 | Sound sensor VCC | Power (do not use 5V) |
| GND | Sound sensor GND | Ground |
| GPIO 25 | Green LED 1 via 220 Ω | Quiet / normal |
| GPIO 26 | Green LED 2 via 220 Ω | Normal |
| GPIO 27 | Green LED 3 via 220 Ω | Normal |
| GPIO 14 | Red LED 1 via 220 Ω | Loud |
| GPIO 13 | Red LED 2 via 220 Ω | Loud |
| GPIO 12 | Buzzer (+) | Loud alert |
| GND | All LED cathodes (short leg), buzzer (−) | Common ground |

Each LED: `GPIO → 220 Ω → LED long leg (anode) → LED short leg (cathode) → GND`.

The sound sensor's digital output **D0 is not used**; the analog output (A0) gives a usable range.

## Notes

- Use a common ground for everything.
- The sensitivity screw on the sensor changes the reading range; recalibrate `QUIET_MAX` / `NORMAL_MAX` in the sketch after adjusting it.
- GPIO 12 is a boot-strapping pin on the ESP32. If upload or boot fails with the buzzer connected, disconnect it while uploading.
