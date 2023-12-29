#define ENA   5         // Enable/speed motors Left         GPIO14(D5)
#define IN_1  2           // L298N in1 motors Left            GPIO2(D4)
#define IN_2  0           // L298N in2 motors Left            GPIO0(D3)
#define RECV_PIN 14



#include <ESP8266WiFi.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>

int speedCar = 1000;         // 0 - 255


IRrecv irrecv(RECV_PIN);
decode_results results;


void setup()
{
  pinMode(ENA, OUTPUT);
  pinMode(IN_1, OUTPUT);
  pinMode(IN_2, OUTPUT);
  Serial.begin(9600);
  irrecv.enableIRIn();
}

void goAhead()
{
  digitalWrite(IN_1, LOW);
  digitalWrite(IN_2, HIGH);
  analogWrite(ENA, speedCar);
}

void goBack() 
{
  digitalWrite(IN_1, HIGH);
  digitalWrite(IN_2, LOW);
  analogWrite(ENA, speedCar);
}





void stopDolly() 
{
  digitalWrite(IN_1, LOW);
  digitalWrite(IN_2, LOW);
  analogWrite(ENA, speedCar);
}

void loop() 
{
  analogWrite(ENA, speedCar);
  
  if ( irrecv.decode(&results) )
  {

   
    
        if(results.value == 0xFF18E7)
        {
        goAhead();
        Serial.println("Forward");
        delay(1000);
         
        }
        
        else if(results.value == 0xFF4AB5)
        {
        goBack();
        Serial.println("Back");
        delay(1000);
         
        }
        
        else if(results.value ==  0xFF38C7)
        {
        stopDolly();
        Serial.println("Stop");
        delay(1000);
         
        }

        else if(results.value ==  0xFF9867)
        {
        speedCar = 25;
        Serial.println(speedCar);
        delay(1000);
         
        }
        
        else if(results.value ==  0xFFA25D)
        {
        speedCar = 50;
        Serial.println(speedCar);
        delay(1000);
         
        }
        
        else if(results.value ==  0xFF629D)
        {
        speedCar = 100;
        Serial.println(speedCar);
        delay(1000);
         
        }
        
        else if(results.value ==  0xFFE21D)
        {
        speedCar = 125;
        Serial.println(speedCar);
        delay(1000);
         
        }

        else if(results.value ==  0xFF22DD)
        {
        speedCar = 150;
        Serial.println(speedCar);
        delay(1000);
         
        }

        else if(results.value ==  0xFF02FD)
        {
        speedCar = 175;
        Serial.println(speedCar);
        delay(1000);
         
        }

        else if(results.value ==  0xFFC23D)
        {
        speedCar = 200;
        Serial.println(speedCar);
        delay(1000);
         
        }

        else if(results.value ==  0xFFE01F)
        {
        speedCar = 215;
        Serial.println(speedCar);
        delay(1000);
         
        }

        else if(results.value ==  0xFFA857)
        {
        speedCar = 230;
        Serial.println(speedCar);
        delay(1000);
         
        }

        else if(results.value ==  0xFF906F)
        {
        speedCar = 250;
        Serial.println(speedCar);
        delay(1000);
         
        }
        
   

    irrecv.resume();
  }

}
