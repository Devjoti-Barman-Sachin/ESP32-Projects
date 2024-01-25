#include <WiFi.h>
#include <WiFiManager.h>

const int flashButtonPin = 0;
const int led = 2;

void setup() {
  Serial.begin(115200);

  pinMode(flashButtonPin, INPUT);
  pinMode(led, OUTPUT);

  digitalWrite(led, LOW);

  WiFiManager wifiMn;

  if (!wifiMn.autoConnect("ESP32")) {
    Serial.println("Failed to connect and hit timeout");
    esp_restart();
    delay(1000);
  }
  Serial.println("Connected to Wi-Fi!");
  digitalWrite(led, HIGH);
}

void loop() {
  if (digitalRead(flashButtonPin) == LOW) {
    WiFiManager wifiMn;
    wifiMn.resetSettings();
    esp_restart();
    Serial.println("WiFi settings reset");
    delay(1000);
  }
  // Your code here
}
