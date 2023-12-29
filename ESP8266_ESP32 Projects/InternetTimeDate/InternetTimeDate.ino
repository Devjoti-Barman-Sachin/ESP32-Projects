#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ctime>

const char *ssid = "DBS";
const char *password = "20000112";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", (6*3600));

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Initialize the NTP client
  timeClient.begin();
}

void loop() {
  // Update the NTP client
  timeClient.update();

  // Get the UNIX timestamp
  unsigned long unixTime = timeClient.getEpochTime();

  // Convert the UNIX timestamp to a tm structure
  struct tm *timeinfo;
  time_t rawtime = (time_t)unixTime;
  timeinfo = localtime(&rawtime);

  // Print the date and time to the console
  char formattedTime[30];

  char dateString[20];
  char timeString[20];
  strftime(dateString, sizeof(formattedTime), "%Y-%m-%d", timeinfo);
  strftime(timeString, sizeof(formattedTime), "%H:%M:%S", timeinfo);
  Serial.println(dateString);
  Serial.println(timeString);

  // Wait for one second
  delay(1000);
}
