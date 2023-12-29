int roadHole = 0;
int carCrash = 0;
int count = 0;


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
#define WIFI_SSID "CarCrash"
#define WIFI_PASSWORD "12345678"

// Insert Firebase project API Key
#define API_KEY "AIzaSyAQa5WzK920BcGmxahjKpvl_jmNaT0Hzlw"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "roadholedetection-default-rtdb.firebaseio.com/"

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

//unsigned long sendDataPrevMillis = 0;
//int count = 0;
bool signupOK = 0;
//_________________________________________________________________________________________________



//-----------------------------------MPU6050------------------------------------
#include <Wire.h>
#include <MPU6050.h>
MPU6050 mpu;
//------------------------------------------------------------------------------


//-----------------------------------BMP180------------------------------------
#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;
//-----------------------------------------------------------------------------


//-----------------------------------GPS NEO-6M------------------------------------
#include "SoftwareSerial.h"
#include <TinyGPS++.h>
SoftwareSerial gpsSerial(16, 17); // RX, TX
TinyGPSPlus gps;
//---------------------------------------------------------------------------------


//-----------------------------------Ultrasonic Sonar------------------------------------
const int trigPin = 13;
const int echoPin = 12;
const int trigPin2 = 26;
const int echoPin2 = 27;
const int trigPin3 = 32;
const int echoPin3 = 33;
long duration;
float distance;
//----------------------------------------------------------------------------------------



void setup() {
  Serial.begin(9600);

  //-----------------------------------MPU6050------------------------------------
  Wire.begin();
  mpu.initialize();
  //------------------------------------------------------------------------------


  //-----------------------------------GPS NEO-6M------------------------------------
  gpsSerial.begin(9600);
  //---------------------------------------------------------------------------------


  //-----------------------------------BMP180------------------------------------
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1);
  }
  //-----------------------------------------------------------------------------


  //-----------------------------------Ultrasonic Sonar------------------------------------
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);

  pinMode(trigPin3, OUTPUT);
  pinMode(echoPin3, INPUT);
  //---------------------------------------------------------------------------------------


  //____________________________________WiFi______________________________________

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");

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
    signupOK = 1;
  }
  else
  {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  // Assign the callback function for the long running token generation task
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(1);
  //___________________________________________________________________________________


}

void loop() {

  //-----------------------------------MPU6050------------------------------------
  int16_t ax, ay, az;
  mpu.getAcceleration(&ax, &ay, &az);

  // Calculate total acceleration vector
  float totalAcc = sqrt(sq(ax) + sq(ay) + sq(az));

  // Adjust this threshold value based on testing
  float crashThreshold = 25000.0; // Adjust this value

  if (totalAcc > crashThreshold) {
    // Potential crash detected
    Serial.println("Crash detected!");
    carCrash = 1;
  }
  Serial.println(totalAcc);
  delay(100);
  //------------------------------------------------------------------------------


  //-----------------------------------BMP180------------------------------------
  float temperature = bmp.readTemperature();
  float pressure = bmp.readPressure() / 100.0; // Convert pressure to hPa (hectopascals)

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" °C\t");

  Serial.print("Pressure: ");
  Serial.print(pressure);
  Serial.println(" hPa");
  //-----------------------------------------------------------------------------


  //-----------------------------------GPS NEO-6M------------------------------------
  while (gpsSerial.available()) {
    gps.encode(gpsSerial.read());
  }
  //---------------------------------------------------------------------------------


  //-----------------------------------Ultrasonic Sonar------------------------------------

  //Sonar1
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;
  Serial.print("Distance1: ");
  Serial.println(distance);
  if (distance >= 1.0 && distance <= 2.00)
  {
    Serial.println("Crash detected!");
    carCrash = 1;
  }

  //Sonar2
  digitalWrite(trigPin2, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin2, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin2, LOW);
  duration = pulseIn(echoPin2, HIGH);
  distance = duration * 0.034 / 2;
  Serial.print("Distance2: ");
  Serial.println(distance);
  if (distance > 10 )
  {
    Serial.println("Crash detected!");
    carCrash = 1;
  }
  else if (distance > 4.5 && distance <= 10 )
  {
    Serial.println("Hole detected!");
    roadHole = 1;
  }

  //Sonar3
  digitalWrite(trigPin3, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin3, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin3, LOW);
  duration = pulseIn(echoPin3, HIGH);
  distance = duration * 0.034 / 2;
  Serial.print("Distance3: ");
  Serial.println(distance);
  if (distance >= 1.0 && distance <= 2.00)
  {
    Serial.println("Crash detected!");
    carCrash = 1;
  }
  //-------------------------------------------------------------------------------------
  if (roadHole == 1)
  {
    Firebase.RTDB.setFloat(&fbdo, "/Temperature", temperature);
    Firebase.RTDB.setFloat(&fbdo, "/Pressure", pressure);

    if (Firebase.RTDB.getInt(&fbdo, "/count"))
    {
      count = fbdo.intData();
      Serial.println(count);
    }

    count++;

    Firebase.RTDB.setInt(&fbdo, "/count", count);
    Firebase.RTDB.setInt(&fbdo, "/hole_id/" + String(count) + "/hole_id", count);
    Firebase.RTDB.setString(&fbdo, "/hole_id/" + String(count) + "/hole_location", "X");
    Firebase.RTDB.setString(&fbdo, "/hole_id/" + String(count) + "/location_lat", String(gps.location.lat(), 6));
    Firebase.RTDB.setString(&fbdo, "/hole_id/" + String(count) + "/location_long", String(gps.location.lng(), 6));
    Firebase.RTDB.setInt(&fbdo, "/Hole", 1);



    delay(6000);
    Firebase.RTDB.setInt(&fbdo, "/Hole", 0);
    roadHole = 0;
  }

  if (carCrash == 1)
  {
    Firebase.RTDB.setInt(&fbdo, "/Crash", 1);
    Firebase.RTDB.setFloat(&fbdo, "/Temperature", temperature);
    Firebase.RTDB.setFloat(&fbdo, "/Pressure", pressure);
    delay(6000);

    Firebase.RTDB.setInt(&fbdo, "/Crash", 0);
    carCrash = 0;
  }

}
