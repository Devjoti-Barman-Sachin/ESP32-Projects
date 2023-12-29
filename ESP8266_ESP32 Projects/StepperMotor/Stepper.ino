//2

#include <AccelStepper.h>

//Test Purpous LED 13
int led = 13;

//Stepper Motor Pin
#define dirPin 2
#define stepPin 3
#define motorInterfaceType 1

//IR(Counter Pin)
int ir1 = 4;
int ir2 = 5;
int relay1 = 6;
int relay2 = 7;

//Total Cash Note Count
int n=0;
int Note_Gone = 0;
int x_check=0;
bool x1, x2;

void(* resetFunc) (void) = 0;

// Create a new instance of the AccelStepper class:
AccelStepper stepper = AccelStepper(motorInterfaceType, stepPin, dirPin);

 
void setup() 
{
  Serial.begin(9600);
  
  // Set the maximum speed in steps per second:
  stepper.setMaxSpeed(1000);

  //IR pin
  pinMode(ir1, INPUT_PULLUP);
  pinMode(ir2, INPUT_PULLUP);
  pinMode(led, OUTPUT);
}
 

void loop() 
{ 
  n=0;
  while (Serial.available()==0){}
  n = Serial.parseInt();  
  
  while( (n>0) && (n!=Note_Gone) && (n!=x_check) )
  {
    digitalWrite(relay1, HIGH);
    digitalWrite(relay2, HIGH);
    
    //Forward Movement
    stepper.setCurrentPosition(0);
    while(stepper.currentPosition() != 200)
    {
      stepper.setSpeed(400);
      stepper.runSpeed();
    }

    while(digitalRead(ir1)!=LOW && digitalRead(ir2)!=LOW)
    {
      x1=digitalRead(ir1);
    }
    
    if(!x1)
    {
      Note_Gone++;
      digitalWrite(led, HIGH);
      delay(50);
      digitalWrite(led, LOW);
    }

    x_check++;
 
    delay(1000);

    //Backword Movement
    stepper.setCurrentPosition(0);
    while(stepper.currentPosition() != -50)
    {
      stepper.setSpeed(-400);
      stepper.runSpeed();
    }

    digitalWrite(relay1, LOW);
    digitalWrite(relay2, LOW);
 
    delay(1000);
  }
  

  //Final Backword Movement
  stepper.setCurrentPosition(0); 
  while(stepper.currentPosition() != -20) 
  {
    stepper.setSpeed(-400);
    stepper.runSpeed();
  }
  delay(1000);
  
  resetFunc();
}
