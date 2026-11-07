# Hardware — ESP32 Classroom Noise Monitor

An ESP32 listens to classroom noise with an analog sound sensor, shows the level locally with LEDs and a buzzer, and sends it to the Flask backend over Wi-Fi so it appears on the dashboard.

## Contents

| Path | What it is |
|---|---|
| `esp32_sound_wifi/esp32_sound_wifi.ino` | **Final build.** Sound sensor + LEDs + buzzer + Wi-Fi push to the backend |
| `esp32_firmware/esp32_firmware.ino` | Earlier reference firmware: HTTP server for backend-to-ESP32 alert commands with an LCD (`/command`, `/status`). Not used in the final demo build. |
| `docs/wiring_diagram.md` | Wiring and pin table for the final build |

## Behaviour (final build)

The sketch measures the **peak-to-peak** sound signal over 200 ms windows.

| Level | State | LEDs | Buzzer |
|---|---|---|---|
| ≤ 100 | QUIET | 1 green | off |
| 101 – 750 | NORMAL | 3 green | off |
| > 750 | LOUD | 2 red | on |

Every 2 seconds it sends `POST /api/devices/esp32/sound` with `{"level": N, "state": "..."}`. The backend marks the ESP32 **online** if data arrived in the last 10 seconds.

Thresholds (`QUIET_MAX`, `NORMAL_MAX`) are constants at the top of the sketch. They were set by hand for one sensor and one room, so adjust them for your own setup.

## Setup

1. Install Arduino IDE 2.x and the **ESP32 board package** (Boards Manager → "esp32").
2. Wire the hardware as in `docs/wiring_diagram.md`.
3. Open `esp32_sound_wifi/esp32_sound_wifi.ino` and set:
   - `WIFI_SSID` and `WIFI_PASSWORD`, your 2.4 GHz Wi-Fi (ESP32 does not support 5 GHz)
   - `SERVER_URL`, with your laptop's IPv4 address (`ipconfig` on Windows)
4. Board: **ESP32 Dev Module**; select the COM port; Upload.
5. Open Serial Monitor at **115200** baud. You should see `WiFi connected` and `Backend reply code: 200`.
6. Start the backend first. Laptop and ESP32 must be on the same Wi-Fi, and Windows Firewall must allow port 5000.

> Never commit your real Wi-Fi password. Keep placeholders in the repository.

## Test without the ESP32

```bash
curl -X POST http://127.0.0.1:5000/api/devices/esp32/sound -H "Content-Type: application/json" -d "{\"level\": 500}"
curl http://127.0.0.1:5000/api/devices/esp32/sound
```
