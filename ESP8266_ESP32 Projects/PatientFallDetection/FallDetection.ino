#define BLYNK_PRINT Serial
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <OneWire.h>
#include <DallasTemperature.h>

OneWire oneWire(D4);
DallasTemperature sensors(&oneWire);

BlynkTimer timer;

const int MPU_addr = 0x68; // I2C address of the MPU-6050
int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;
float ax = 0, ay = 0, az = 0, gx = 0, gy = 0, gz = 0;
boolean fall = false; //stores if a fall has occurred
boolean trigger1 = false; //stores if first trigger (lower threshold) has occurred
boolean trigger2 = false; //stores if second trigger (upper threshold) has occurred
boolean trigger3 = false; //stores if third trigger (orientation change) has occurred
byte trigger1count = 0; //stores the counts past since trigger 1 was set true
byte trigger2count = 0; //stores the counts past since trigger 2 was set true
byte trigger3count = 0; //stores the counts past since trigger 3 was set true
int angleChange = 0;

char auth[] = "kFIx-Z3NQe1uuJVNMfXhaoGyhk2yVdtZ";
char ssid[] = "3MobileWiFi-C997";
char pass[] = "0jr8555d";


float temp = 0;


void setup()
{
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  sensors.begin();
  timer.setInterval(1250L, SendTemps);

}

void SendTemps()
{
  sensors.requestTemperatures();
  temp = sensors.getTempFByIndex(0);

  Blynk.virtualWrite(V1, temp);
}





void loop()
{
  Blynk.run();
  timer.run();


  mpu_read();
  ax = (AcX - 2050) / 16384.00;
  ay = (AcY - 77) / 16384.00;
  az = (AcZ - 1947) / 16384.00;
  gx = (GyX + 270) / 131.07;
  gy = (GyY - 351) / 131.07;
  gz = (GyZ + 136) / 131.07;

  Serial.println(ax);
  Serial.println(ay);
  Serial.println(az);
  Serial.println(Tmp);
  Serial.println(gx);
  Serial.println(gy);
  Serial.println(gz);


  // calculating Amplitute vactor for 3 axis
  float Raw_Amp = pow(pow(ax, 2) + pow(ay, 2) + pow(az, 2), 0.5);
  int Amp = Raw_Amp * 10;  // Mulitiplied by 10 bcz values are between 0 to 1

  if (Amp <= 2 && trigger2 == false) { //if AM breaks lower threshold (0.4g)
    trigger1 = true;

  }
  if (trigger1 == true) {
    trigger1count++;
    if (Amp >= 10) { //if AM breaks upper threshold (3g)
      trigger2 = true;

      trigger1 = false; trigger1count = 0;
    }
  }
  if (trigger2 == true) {
    trigger2count++;
    angleChange = pow(pow(gx, 2) + pow(gy, 2) + pow(gz, 2), 0.5); 
    if (angleChange >= 30 && angleChange <= 400) { //if orientation changes by between 80-100 degrees
      trigger3 = true; trigger2 = false; trigger2count = 0;
      Serial.println("...............");
      Serial.println(angleChange);
      Serial.println("...............");

    }
  }
  if (trigger3 == true) {
    trigger3count++;
    if (trigger3count >= 5) {        
      angleChange = pow(pow(gx, 2) + pow(gy, 2) + pow(gz, 2), 0.5);
      //delay(10);

      if ((angleChange >=0) && (angleChange <= 20)) { //if orientation changes remains between 0-10 degrees
        fall = true; trigger3 = false; trigger3count = 0;
        Serial.println("**********");
        Serial.println(angleChange);
        Serial.println("**********");
      }
      else { //user regained normal orientation
        trigger3 = false; trigger3count = 0;

      }
    }
  }
  if (fall == true) { //in event of a fall detection
    String msgBody = "Fall Detected…! take action immediately. Patient Body Temperature: ";
    msgBody.concat(temp);

    Blynk.notify("Alert : Fall Detected…! take action immediately.");

    Blynk.email("arifsheikh286658@gmail.com", "Alert : Fall Detected…!", msgBody.c_str());

    Serial.println("FALL DETECTED");
    fall = false;
  }
  if (trigger2count >= 20) { //allow 0.5s for orientation change
    trigger2 = false; trigger2count = 0;

  }
  if (trigger1count >= 20) { //allow 0.5s for AM to break upper threshold
    trigger1 = false; trigger1count = 0;

  }
  delay(90);

}


void mpu_read() {
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr, 14, true); // request a total of 14 registers
  AcX = Wire.read() << 8 | Wire.read(); // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
  AcY = Wire.read() << 8 | Wire.read(); // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ = Wire.read() << 8 | Wire.read(); // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp = Wire.read() << 8 | Wire.read(); // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyX = Wire.read() << 8 | Wire.read(); // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY = Wire.read() << 8 | Wire.read(); // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ = Wire.read() << 8 | Wire.read(); // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)



}
