#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char auth[] = "_8PpBUTjGUahPT-nkzwkMb0QSZoSGYyf

";
 
// Your WiFi credentials.

char ssid[] = "ATM";
char pass[] = "miniatm2021";
 
  int relay1 = 12;  // D6 pin 
  int relay2 = 14;  // D2 pin 
  int relay3 = 4;   // D1 pin 
  int relay4 = 5;   // D5 pin 
 
void setup()
{
  
  Serial.begin(115200);
 
  pinMode(relay1,OUTPUT);
  pinMode(relay2,OUTPUT);
  pinMode(relay3,OUTPUT);
  pinMode(relay4,OUTPUT);
 
  digitalWrite(relay1, LOW);
  digitalWrite(relay2, LOW);
  digitalWrite(relay3, LOW);
  digitalWrite(relay4, LOW);
 
  Blynk.begin(auth, ssid, pass);
  
}
 
void loop()
{
  Blynk.run();
}
