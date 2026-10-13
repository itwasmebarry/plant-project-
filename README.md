# plant-project-

Microcontroller projects built on the Seeed Studio XIAO ESP32 boards.

## Projects

| Folder | What it does | Board |
|---|---|---|
| [`ESP32-Blynk-Servo/`](ESP32-Blynk-Servo/) | Controls a 360° servo from your phone, from anywhere, using the Blynk IoT app | XIAO ESP32-S3 (or ESP32-C3) |

---

## ESP32-Blynk-Servo at a glance

```
Phone (Blynk app) → Blynk cloud → Home WiFi → XIAO ESP32-S3 → Servo (pin D3)
```

### What the code does (`servo_blynk/servo_blynk.ino`)

| Part of the code | Behaviour |
|---|---|
| `setup()` | Attaches the servo to **D3** with a 1000–2000 µs pulse range, sets it to stop, then connects to WiFi and Blynk |
| `BLYNK_WRITE(V0)` | **Speed slider.** Converts −100…100 from the app to 0…180 for the servo: −100 is full speed one way, 0 is stop, 100 is full speed the other way |
| `BLYNK_WRITE(V1)` | **Stop button.** Stops the servo and sets the app slider back to 0 |
| `BLYNK_CONNECTED()` | Each time the board connects or reconnects, stops the servo and sets the slider to 0, so it never starts moving on its own |
| `loop()` | Runs Blynk and stops the servo if the connection drops (failsafe) |
| `STOP_POINT` | The value that stops the servo (90). Change it to 88–92 if the servo creeps when it should be still |

### Blynk virtual pins

| Pin | Widget | Range |
|---|---|---|
| V0 | Slider (Speed) | −100 to 100, default 0 |
| V1 | Push button (Stop) | 0 / 1 |

### Wiring

| From | To |
|---|---|
| 5 V supply (+) | XIAO **5V** and servo **red** |
| GND (−) | XIAO **GND** and servo **brown/black** |
| XIAO **D3** | Servo **orange/yellow** (signal) |

All grounds must be connected together. Don't power the servo from the 3.3 V pin.

### Quick start

1. Install the **esp32** board package and the **ESP32Servo** and **Blynk** libraries in the Arduino IDE.
2. Copy `ESP32-Blynk-Servo/servo_blynk/secrets_example.h` to `secrets.h` in the same folder.
3. Fill in your Blynk template ID, template name, auth token and WiFi details (2.4 GHz only).
4. Open `servo_blynk.ino`, select your XIAO board and port, and upload.

For the full guide (parts list, Blynk setup, how PWM works and troubleshooting), see [`ESP32-Blynk-Servo/README.md`](ESP32-Blynk-Servo/README.md).

## Keeping credentials private

Your real `secrets.h` is listed in `.gitignore` and is never committed. Only `secrets_example.h`, which has placeholder values, is stored in the repo. Treat your Blynk auth token like a password.
