# ESP32 Blynk Servo Controller

Control a 360° (continuous rotation) servo from your phone, from anywhere in the world, using a Seeed Studio XIAO ESP32-S3 and the Blynk IoT app.

```
Phone (Blynk app) → Blynk cloud → Home WiFi → XIAO ESP32-S3 → Servo
                                                     ↑
                                          HW-131 breadboard PSU (5 V)
```

---

## 1. Hardware

| Part | Notes |
|---|---|
| Seeed Studio XIAO ESP32-S3 | XIAO ESP32-C3 also works (same pin labels) |
| 360° continuous rotation servo | e.g. SG90/MG90S "360" variant |
| HW-131 (MB102) breadboard power supply | Powered by USB or 6.5–12 V DC barrel jack |
| Breadboard + jumper wires | |
| USB-C data cable | Some cables are charge-only and won't work for uploading |
| Optional: 100–470 µF capacitor | Across 5 V and GND near the ESP32, stops WiFi brown-out resets |

## 2. Wiring

Set the HW-131 jumper on the side you use to **5 V**.

| From | To |
|---|---|
| HW-131 5 V rail (+) | XIAO **5V** pin (VBUS) |
| HW-131 5 V rail (+) | Servo **red** wire |
| HW-131 GND rail (−) | XIAO **GND** |
| HW-131 GND rail (−) | Servo **brown/black** wire |
| XIAO **D3** | Servo **orange/yellow** (signal) wire |

Rules:
- **All grounds must be connected together.**
- Never power the servo from the XIAO's 3.3 V pin. Servos need 5 V and draw current spikes.
- **Disconnect the 5 V wire to the XIAO whenever USB-C is plugged in** (e.g. while uploading). Don't feed it from two sources at once.
- If the breadboard's power rail has a gap in the middle (split rail), bridge it with a jumper wire.
- Pins to avoid for the servo on the S3: D2 (boot-strapping pin). D3 is safe.

## 3. Software setup (Arduino IDE)

1. Install **Arduino IDE 2.x** from arduino.cc.
2. **Boards Manager** → search `esp32` → install **esp32 by Espressif Systems**.
3. **Library Manager** → install:
   - **ESP32Servo** by Kevin Harrington
   - **Blynk** by Volodymyr Shymanskyy
4. **Tools → Board → esp32 → XIAO_ESP32S3** (or XIAO_ESP32C3 if that's your board).
5. **Tools → USB CDC On Boot → Enabled** (needed for Serial Monitor output on the S3).
6. **Tools → Port** → pick the port that appears when the board is plugged in.

## 4. Blynk setup

### On blynk.cloud (computer)
1. Create a free account.
2. **Developer Zone → My Templates → New Template**
   - Name: `Servo Control`, Hardware: `ESP32`
3. In the template, open **Datastreams** (click **Edit** first) and add two **Virtual Pin** datastreams:

   | Name | Pin | Type | Min | Max | Default |
   |---|---|---|---|---|---|
   | Speed | V0 | Integer | -100 | 100 | 0 |
   | Stop | V1 | Integer | 0 | 1 | 0 |

4. Click **Save** (top right). Nothing applies until you save.
5. **Devices → New Device → From template → Servo Control**.
6. Copy the three lines it shows (`BLYNK_TEMPLATE_ID`, `BLYNK_TEMPLATE_NAME`, `BLYNK_AUTH_TOKEN`).

### In the Blynk IoT phone app
1. Log in with the same account and open your device.
2. Tap the **wrench** (turn on Developer Mode in settings if it's missing).
3. Add a **Slider** → Datastream **V0 (Speed)**.
4. Add a **Button** → Datastream **V1 (Stop)**, Mode **Push**.
5. Exit edit mode.

## 5. Configure and upload

1. In `servo_blynk/`, copy `secrets_example.h` to **`secrets.h`**.
2. Fill in your Blynk lines and WiFi name/password in `secrets.h`. WiFi must be **2.4 GHz**.
3. Open `servo_blynk/servo_blynk.ino` in Arduino IDE and click **Upload**.
4. Press **R** (reset) on the XIAO.

`secrets.h` is in `.gitignore`, so your auth token and WiFi password are never pushed to GitHub. Treat the auth token like a password: anyone who has it can control your device.

## 6. Using it

- Power the board from the HW-131 (no laptop needed). After a few seconds it shows **Online** in Blynk.
- Slider at **0** = stopped. Drag left/right = spin one way/the other, faster towards the ends.
- **Stop** button = immediate stop and slider resets to 0.
- Test "from anywhere" by switching your phone to mobile data.

## 7. How it works

### Pulse Width Modulation (PWM)
A microcontroller pin can only be fully on (3.3 V) or fully off (0 V). PWM switches the pin on and off very fast; the percentage of time it's on is the **duty cycle**. LEDs and motors feel the average.

### Servo signal
A servo instead measures **how long each pulse lasts**. The signal repeats every 20 ms, with a pulse of 1–2 ms:

| Pulse | 360° servo | 180° servo | Code |
|---|---|---|---|
| 1.0 ms | Full speed one way | One end | `write(0)` |
| 1.5 ms | **Stop** | Middle | `write(90)` |
| 2.0 ms | Full speed other way | Other end | `write(180)` |

`myServo.attach(D3, 1000, 2000)` sets the 1000–2000 µs range so that `write(90)` is exactly 1.5 ms. Without it the library's default range makes `write(90)` slightly short and the servo creeps.

A 360° servo has **no position feedback**. You control speed and direction, not where it ends up.

### Data flow
Slider (widget) → V0 (datastream) → Blynk cloud → `BLYNK_WRITE(V0)` in the code → `map()` −100..100 to 0..180 → `myServo.write()` → pulse width → servo speed.

### Safety
- `BLYNK_CONNECTED()` stops the servo on every (re)connect.
- `loop()` stops the servo if the Blynk connection drops.

## 8. Troubleshooting

| Problem | Fix |
|---|---|
| `ESP32Servo.h: No such file or directory` | Install the ESP32Servo library |
| `Could not open COMx ... doesn't exist` | Wrong port. Reselect in Tools → Port; unplug/replug to see which one appears |
| `Could not open COMx ... Access is denied` | Port in use. Close the Serial Monitor / other IDE windows, then upload |
| No port appears | Use a data USB-C cable. Or hold **B**, plug in, release, then upload |
| Board selected is S3 but you have a C3 (or vice versa) | Tools → Board → pick the matching XIAO |
| Serial Monitor only shows `ESP-ROM:...` | Tools → USB CDC On Boot → Enabled, re-upload |
| Serial Monitor says "Not connected" | The board must be on USB for the monitor to work. Check Blynk for Online status instead |
| Device stays Offline | Check WiFi name/password (case-sensitive), 2.4 GHz band, correct auth token |
| Servo creeps when it should stop | Change `STOP_POINT` to 88–92, or turn the servo's trim screw |
| Servo ignores the slider | Widget not linked to V0, or V0 range not −100..100 |
| Datastream list empty in app | Datastreams not saved on the template, or device not created from that template |
| Board keeps resetting | Add a 100–470 µF capacitor across 5 V and GND; make sure the servo is on the 5 V rail |
| No LED on the XIAO | Normal. It has no power LED, only a charge LED |

## 9. Possible next steps
- Port to **MQTT** (e.g. HiveMQ Cloud + an MQTT dashboard app) to avoid Blynk free-tier limits.
- Add WiFiManager so WiFi can be changed without re-uploading.
- Swap to a 180° servo or stepper motor for position control.
