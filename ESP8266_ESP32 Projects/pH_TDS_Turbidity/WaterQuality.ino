//_______________________________________________________WiFi and Firebase___________________________________________
#include <Arduino.h>
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>


//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert your network credentials
#define WIFI_SSID "Galaxy M31"
#define WIFI_PASSWORD "20000112"

// Insert Firebase project API Key
#define API_KEY "AIzaSyCzJ1Y8_zyjytq6Aj98hNIQS6cgosEa2wo"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "water-quality-monitor-59507-default-rtdb.firebaseio.com/"

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

//unsigned long sendDataPrevMillis = 0;
//int count = 0;
bool signupOK = false;
//_________________________________________________________________________________________________




//--------------------------------------------------TDS Sensor----------------------------------------------------------

#define tdsPin 32
#define VREF 3.3             // analog reference voltage(Volt) of the ADC
#define SCOUNT  3            // sum of sample point

int analogBuffer[SCOUNT];     // store the analog value in the array, read from ADC
int analogBufferTemp[SCOUNT];
int analogBufferIndex = 0;
int copyIndex = 0;

float averageVoltage = 0;
float tdsValue = 0;
float temperature = 28;       // current temperature for compensation

// median filtering algorithm
int getMedianNum(int bArray[], int iFilterLen) {
  int bTab[iFilterLen];
  for (byte i = 0; i < iFilterLen; i++)
    bTab[i] = bArray[i];
  int i, j, bTemp;
  for (j = 0; j < iFilterLen - 1; j++) {
    for (i = 0; i < iFilterLen - j - 1; i++) {
      if (bTab[i] > bTab[i + 1]) {
        bTemp = bTab[i];
        bTab[i] = bTab[i + 1];
        bTab[i + 1] = bTemp;
      }
    }
  }
  if ((iFilterLen & 1) > 0) {
    bTemp = bTab[(iFilterLen - 1) / 2];
  }
  else {
    bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
  }
  return bTemp;
}

//--------------------------------------------------------------------------------------------------------------------------

//-----------------------------------------------Turbiduty----------------------------------------------

#define turbidutyPin 35
int read_ADC;
int ntu;

//------------------------------------------------------------------------------------------------



//-----------------------------------------------pH Sensor-----------------------------------------

#define phPin 34
int sensorValue = 0;
unsigned long int avgValue;
float b;
int buf[10], temp;

//--------------------------------------------------------------------------------------------------


void setup()
{

  Serial.begin(115200);


  //____________________________________WiFi______________________________________

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");


  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }


  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  //______________________________________________________________________________


  //_____________________________________Firebase__________________________________

  // Assign the api key (required)
  config.api_key = API_KEY;

  // Assign the RTDB URL (required)
  config.database_url = DATABASE_URL;

  // Sign up
  if (Firebase.signUp(&config, &auth, "", ""))
  {
    Serial.println("Signup Successfull");
    signupOK = true;
  }
  else
  {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  // Assign the callback function for the long running token generation task
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  //___________________________________________________________________________________




  //--------------------------------------------------TDS Sensor----------------------------------------------------------

  pinMode(tdsPin, INPUT);

  //-----------------------------------------------------------------------------------------------------------------------


  //-----------------------------------------------Turbiduty----------------------------------------------

  pinMode(turbidutyPin, INPUT);

  //------------------------------------------------------------------------------------------------


}

void loop()
{

  //-----------------------------------------------Turbiduty------------------------------------------
  read_ADC = analogRead(turbidutyPin);
  if (read_ADC > 208)read_ADC = 208;

  ntu = map(read_ADC, 0, 208, 300, 0);


  // Write humidity on the database
  if (Firebase.RTDB.setFloat(&fbdo, "live-data/turbidity", ntu))
  {
    Serial.print("Turbidity: ");
    Serial.println(ntu);

    if (ntu < 10)
      Serial.println("Water Very Clean");

    if (ntu >= 10 && ntu < 30)
      Serial.println("Water Norm Clean");

    if (ntu >= 30)
      Serial.println("Water Very Dirty");

  }
  else
  {
    Serial.println("FAILED");
    Serial.println("REASON: " + fbdo.errorReason());
  }

  delay(200);
  //----------------------------------------------------------------------------------------------------




  //--------------------------------------------------TDS Sensor----------------------------------------------------------

  static unsigned long analogSampleTimepoint = millis();
  if (millis() - analogSampleTimepoint > 40U) { //every 40 milliseconds,read the analog value from the ADC
    analogSampleTimepoint = millis();
    analogBuffer[analogBufferIndex] = analogRead(tdsPin);    //read the analog value and store into the buffer
    analogBufferIndex++;
    if (analogBufferIndex == SCOUNT) {
      analogBufferIndex = 0;
    }
  }

  static unsigned long printTimepoint = millis();
  if (millis() - printTimepoint > 800U)
  {
    printTimepoint = millis();
    for (copyIndex = 0; copyIndex < SCOUNT; copyIndex++) {
      analogBufferTemp[copyIndex] = analogBuffer[copyIndex];

      // read the analog value more stable by the median filtering algorithm, and convert to voltage value
      averageVoltage = getMedianNum(analogBufferTemp, SCOUNT) * (float)VREF / 1024.0;

      //temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0));
      float compensationCoefficient = 1.0 + 0.02 * (temperature - 25.0);
      //temperature compensation
      float compensationVoltage = averageVoltage / compensationCoefficient;

      //convert voltage value to tds value
      tdsValue = (133.42 * compensationVoltage * compensationVoltage * compensationVoltage - 255.86 * compensationVoltage * compensationVoltage + 857.39 * compensationVoltage) * 0.5;

       

      // Write humidity on the database
      if (Firebase.RTDB.setFloat(&fbdo, "live-data/TDS", tdsValue))
      {
        Serial.print("TDS Value:");
        Serial.print(tdsValue);
        Serial.println("ppm");

      }
      else
      {
        Serial.println("FAILED");
        Serial.println("REASON: " + fbdo.errorReason());
      }

    }
  }

  //---------------------------------------------------------------------------------------------------------------------------


  //------------------------------------------------pH Sensor-----------------------------------------

  for (int i = 0; i < 10; i++)
  {
    buf[i] = analogRead(phPin);
    delay(10);
  }
  for (int i = 0; i < 9; i++)
  {
    for (int j = i + 1; j < 10; j++)
    {
      if (buf[i] > buf[j])
      {
        temp = buf[i];
        buf[i] = buf[j];
        buf[j] = temp;
      }
    }
  }
  avgValue = 0;
  for (int i = 2; i < 8; i++)
    avgValue += buf[i];
  float pHVol = (float)avgValue * 3.3 / 4095 / 6;
  float phValue = -5.70 * pHVol + 21.34;

  Serial.print("PH sensor = ");
  Serial.println(phValue);
  
  // Write humidity on the database

  if (Firebase.RTDB.setFloat(&fbdo, "live-data/ph", phValue))
  {
    Serial.print("PH sensor = ");
    Serial.println(phValue);

  }
  else
  {
    Serial.println("FAILED");
    Serial.println("REASON: " + fbdo.errorReason());
  }


  delay(20);

  //------------------------------------------------------------------------------------------------




}
