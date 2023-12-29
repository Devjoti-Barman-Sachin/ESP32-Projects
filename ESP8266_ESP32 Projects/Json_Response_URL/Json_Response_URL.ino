#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "Galaxy M31";
const char* password = "20000112";


void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  // Make a request to the API
  HTTPClient http;
  String url = "http://13.233.140.165/api/device_mode_check?device_id=17c2fcb11413515d";
  http.begin(url);

  // Check for a response
  int httpCode = http.GET();
  if (httpCode == HTTP_CODE_OK) {
    // Parse the JSON response
    String response = http.getString();
    Serial.println("Response: " + response);

    // Parse JSON using ArduinoJson library
    DynamicJsonDocument jsonDoc(1024);
    deserializeJson(jsonDoc, response);

    // Access the "mode" key and get the last character
    const char* mode = jsonDoc["mode"];
    char lastCharacter = mode[strlen(mode) - 1];

    int devMode = lastCharacter.toInt();

    Serial.println("Last Character of Mode: " + String(lastCharacter));
  } else {
    Serial.println("Failed to get response. HTTP error code: " + String(httpCode));
  }

  http.end();
  delay(5000);  // Wait for 5 seconds before making the next request
}
