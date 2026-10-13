// ESP32 Blynk Servo Controller
// Controls a 360-degree (continuous rotation) servo from the Blynk phone app, from anywhere.
// Board: Seeed Studio XIAO ESP32-S3 (also works on XIAO ESP32-C3 and most ESP32 boards)

// secrets.h holds your Blynk IDs and WiFi details. It is NOT uploaded to GitHub.
// Copy secrets_example.h to secrets.h and fill it in. It must be included first,
// because the Blynk #defines have to come before the Blynk library is included.
#include "secrets.h"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <ESP32Servo.h>

// ---------------- Settings ----------------
const int SERVO_PIN  = D3;    // servo signal wire
const int STOP_POINT = 90;    // write() value that stops the servo; try 88-92 if it creeps
// ------------------------------------------

Servo myServo;

// V0 slider: -100 (full speed one way) ... 0 (stop) ... 100 (full speed other way)
BLYNK_WRITE(V0) {
  int speed = param.asInt();
  int value = map(speed, -100, 100, 0, 180);
  myServo.write(value);
}

// V1 push button: emergency stop
BLYNK_WRITE(V1) {
  if (param.asInt() == 1) {
    myServo.write(STOP_POINT);
    Blynk.virtualWrite(V0, 0);   // reset the slider in the app
  }
}

// On every (re)connection, start in a safe stopped state
BLYNK_CONNECTED() {
  myServo.write(STOP_POINT);
  Blynk.virtualWrite(V0, 0);
}

void setup() {
  Serial.begin(115200);

  // 1000-2000 microsecond pulse range so that write(90) = exactly 1.5 ms = stop
  myServo.attach(SERVO_PIN, 1000, 2000);
  myServo.write(STOP_POINT);

  Blynk.begin(BLYNK_AUTH_TOKEN, WIFI_SSID, WIFI_PASS);
}

void loop() {
  Blynk.run();

  // Failsafe: if the connection drops, stop the servo
  if (!Blynk.connected()) {
    myServo.write(STOP_POINT);
  }
}
