 #include <ESP8266WiFi.h>
 #include "DHT.h"        // DHT11 temperature and humidity sensor Predefined library
 
#define DHTTYPE DHT11   // DHT 11
#define dht_dpin 0      //GPIO-0 D3 pin of nodemcu

int soil       = A0;      //Analog channel A0 as used to measure moisture
int relay = 13;       // GPIO13---D7 of NodeMCU--Motor connection



DHT dht(dht_dpin, DHTTYPE); 

void setup(void)
{ 
  dht.begin();
  Serial.begin(19200);
  
  delay(1000);
  pinMode(relay, OUTPUT);       //D7 as output
  digitalWrite(relay, LOW);     //Deactivate relay
  
}



void loop() {
  
 float h =0.0;  //Humidity level
 float t =0.0;  //Temperature in celcius 
 float f =0.0;  //Temperature in fahrenheit
 float percentage = 0.0; // Calculating percentage of moisture
 float reading    = 0.0; //Analog channel moisture read
 
  
     h = dht.readHumidity();    //Read humidity level
     t = dht.readTemperature(); //Read temperature in celcius
     f = (t * 1.8) + 32;        //Temperature converted to Fahrenheit
     
     reading = analogRead(soil); //Analog pin reading output voltage by water moisture soil sensor
     percentage = ((1024-reading)/674) * 100;   //Converting the raw value in percentage

     Serial.println("*************Reading***************");
     
     Serial.print("Humidity: ");
     Serial.println(h);
     
     Serial.print("Temperature(C): ");
     Serial.println(t);

     Serial.print("Temperature(F): ");
     Serial.println(f);

     Serial.print("Soil Moisture: ");
     Serial.print(percentage);
     Serial.println(" %");

     Serial.print("Soil Sensor Reading: ");
     Serial.println(reading);
     
    if (reading<=490) // If less mositure in soil start the motor otherwise stop
    {  
      digitalWrite(relay, HIGH);
    }
    else 
    {
      digitalWrite(relay, LOW);
    }

    delay(2000);

    Serial.println("");
    
}
