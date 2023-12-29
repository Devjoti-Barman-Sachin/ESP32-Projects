#include <TinyGPSPlus.h>
TinyGPSPlus gps;


void setup() {
  Serial.begin(115200);
  Serial2.begin(115200);
  delay(3000);
}


void loop() {
  while (Serial2.available() > 0)
    if (gps.encode(Serial2.read()))
      displayInfo();
      
  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    while (true);
  }
}

void displayInfo()
{
  Serial.print(F("Location: "));
  if (gps.location.isValid()){
    Serial.print("Lat: ");
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print("Lng: ");
    Serial.print(gps.location.lng(), 6);
    Serial.println();
  }  
  else
  {
    Serial.print(F("INVALID"));
  }
}
