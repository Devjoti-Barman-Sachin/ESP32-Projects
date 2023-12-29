int sound =34;

void setup(){
  Serial.begin(115200);
}

void loop(){
  Serial.println(analogRead(sound));
  
 }
