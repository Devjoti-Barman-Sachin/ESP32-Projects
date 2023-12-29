//________________________________________________________PMS5003__________________________________________________________
#include "PMS.h"
#include "SoftwareSerial.h"

SoftwareSerial pmsSerial(13, 12); // TX, RX

PMS pms(pmsSerial);
PMS::DATA data;
//__________________________________________________________________________________________________________________________



void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);

  pmsSerial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:

  pmsSerial.listen();
  pms.readUntil(data);

  float pm1_0 = data.PM_AE_UG_1_0;
  float pm2_5 = data.PM_AE_UG_2_5;
  float pm10_0 = data.PM_AE_UG_10_0;

  Serial.print("PM 1.0 (ug/m3): ");
  Serial.println(pm1_0);
  Serial.print("PM 2.5 (ug/m3): ");
  Serial.println(pm2_5);
  Serial.print("PM 10.0 (ug/m3): ");
  Serial.println(pm10_0);
  Serial.println();

}
