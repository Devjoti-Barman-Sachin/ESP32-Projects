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
#define API_KEY "AIzaSyDnw2V-1N5iJtn-doy7mWBBQmbaLk0V4DY"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "airquality-6b804-default-rtdb.firebaseio.com/"

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

//unsigned long sendDataPrevMillis = 0;
//int count = 0;
bool signupOK = false;
//_________________________________________________________________________________________________



//________________________________________________________CCS811__________________________________________________________

#include <Wire.h>
#include "ccs811.h"  
CCS811 ccs811(D3);

//__________________________________________________________________________________________________________________________



//________________________________________________________PMS5003__________________________________________________________
#include "PMS.h"
#include "SoftwareSerial.h"

SoftwareSerial pmsSerial(13, 12);

PMS pms(pmsSerial);
PMS::DATA data;
//__________________________________________________________________________________________________________________________



//_______________________________________________________________________________________________________________________


//________________________________________________________DHT22__________________________________________________________
#include "DHT.h"
#define DHTPIN D5
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);
//__________________________________________________________________________________________________________________________

//________________________________________________________DS3231__________________________________________________________

#include <RTClib.h>

RTC_DS3231 rtc;


//__________________________________________________________________________________________________________________________


void setup() 
{
  Serial.begin(115200);

//_____________________________________CCS811___________________________________
    
    Wire.begin();
    // Enable CCS811
    ccs811.set_i2cdelay(50); // Needed for ESP8266 because it doesn't handle I2C clock stretch correctly
    bool ok= ccs811.begin();
    if( !ok ) Serial.println("setup: CCS811 begin FAILED");
  
    // Print CCS811 versions
    Serial.print("setup: hardware    version: "); Serial.println(ccs811.hardware_version(),HEX);
    Serial.print("setup: bootloader  version: "); Serial.println(ccs811.bootloader_version(),HEX);
    Serial.print("setup: application version: "); Serial.println(ccs811.application_version(),HEX);
    
    // Start measuring
    ok= ccs811.start(CCS811_MODE_1SEC);
    if( !ok ) Serial.println("setup: CCS811 start FAILED");
  
//_____________________________________________________________________________


//_____________________________________PMS5003___________________________________
    pmsSerial.begin(9600);
//_____________________________________________________________________________



//_____________________________________DHT22___________________________________
  pinMode(DHTPIN, INPUT);
  dht.begin();
//_____________________________________________________________________________



//____________________________________DS3231___________________________________

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

//______________________________________________________________________________

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


}

void loop()
{
  DateTime now = rtc.now();

  pmsSerial.listen();
  pms.readUntil(data);


  float co;
  uint16_t co2, tvoc, errstat, raw;
  ccs811.read(&co2,&tvoc,&errstat,&raw); 
  
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float pm1_0 = data.PM_AE_UG_1_0;
  float pm2_5 = data.PM_AE_UG_2_5;
  float pm10_0 = data.PM_AE_UG_10_0;

  

  

  int sensorValue = analogRead(A0);
  float voltage = sensorValue * (3.3 / 1023.0); // Convert analog value to voltage
  co = (voltage - 0.1) / 0.2;  // Convert voltage to parts per million (PPM) using the MQ-7 calibration curve


    

  // Convert the day, month, and year to strings
  String dayString = String(now.day());
  String monthString = String(now.month());
  String yearString = String(now.year());

  // Pad the day and month strings with leading zeros if necessary
  if (dayString.length() < 2) {
    dayString = "0" + dayString;
  }
  if (monthString.length() < 2) {
    monthString = "0" + monthString;
  }

  // Build the date string in the format of dd-mm-yyyy
  String date = dayString + "-" + monthString + "-" + yearString;
  

  if (Firebase.ready() && signupOK )
  {

      
    float h_sum = 0;
    float t_sum = 0;
    float pm1_0_sum = 0;
    float pm2_5_sum = 0;
    float pm10_0_sum = 0;
    float co2_sum = 0;
    float co_sum = 0;
    float tvoc_sum = 0;
    float count = 0;


    if (Firebase.RTDB.getFloat(&fbdo,"/"+date+"/COSum")) 
    {
      co_sum = fbdo.floatData();
      co_sum = (co_sum+co);
      Firebase.RTDB.setFloat(&fbdo, date+"/COSum", co_sum);
    } 
    else
    {
      Firebase.RTDB.setFloat(&fbdo, date+"/COSum", 0);
      Serial.println(co_sum);
      co_sum = 0;
    }


    

    if (Firebase.RTDB.getFloat(&fbdo,"/"+date+"/CO2Sum")) 
    {
      co2_sum = fbdo.floatData();
      co2_sum = (co2_sum+co2);
      Firebase.RTDB.setFloat(&fbdo, date+"/CO2Sum", co2_sum);
    } 
    else
    {
      Firebase.RTDB.setFloat(&fbdo, date+"/CO2Sum", 0);
      Serial.println(co2_sum);
      co2_sum = 0;
    }


    if (Firebase.RTDB.getFloat(&fbdo,"/"+date+"/TVOCSum")) 
    {
      tvoc_sum = fbdo.floatData();
      tvoc_sum = (tvoc_sum+tvoc);
      Firebase.RTDB.setFloat(&fbdo, date+"/TVOCSum", tvoc_sum);
    } 
    else
    {
      Firebase.RTDB.setFloat(&fbdo, date+"/TVOCSum", 0);
      Serial.println(tvoc_sum);
      tvoc_sum = 0;
    }
    
    
    if (Firebase.RTDB.getFloat(&fbdo,"/"+date+"/HumiditySum")) 
    {
      h_sum = fbdo.floatData();
      h_sum = (h_sum+h);
      Firebase.RTDB.setFloat(&fbdo, date+"/HumiditySum", h_sum);
    } 
    else
    {
      Firebase.RTDB.setFloat(&fbdo, date+"/HumiditySum", 0);
      Serial.println(h_sum);
      h_sum = 0;
    }

    
    if (Firebase.RTDB.getFloat(&fbdo,"/"+date+"/TemperatureSum"))
    {
      t_sum = fbdo.floatData();
      t_sum = (t_sum+t);
      Firebase.RTDB.setFloat(&fbdo, date+"/TemperatureSum", t_sum);
    }
    else
    {
      Firebase.RTDB.setFloat(&fbdo, date+"/TemperatureSum", 0);
      t_sum = 0;
    }

    
    if (Firebase.RTDB.getFloat(&fbdo,"/"+date+"/PM_1_0_Sum"))
    {
      pm1_0_sum = fbdo.floatData();
      pm1_0_sum = (pm1_0_sum+pm1_0);
      Firebase.RTDB.setFloat(&fbdo, date+"/PM_1_0_Sum", pm1_0_sum);
    }
    else
    {
      Firebase.RTDB.setFloat(&fbdo, date+"/PM_1_0_Sum", 0);
      pm1_0_sum = 0;
    }


    if (Firebase.RTDB.getFloat(&fbdo,"/"+date+"/PM_2_5_Sum"))
    {
      pm2_5_sum = fbdo.floatData();
      pm2_5_sum = (pm2_5_sum+pm2_5);
      Firebase.RTDB.setFloat(&fbdo, date+"/PM_2_5_Sum", pm2_5_sum);
    }
    else
    {
      Firebase.RTDB.setFloat(&fbdo, date+"/PM_2_5_Sum", 0);
      pm2_5_sum = 0;
    }


    if (Firebase.RTDB.getFloat(&fbdo,"/"+date+"/PM_10_0_Sum"))
    {
      pm10_0_sum = fbdo.floatData();
      pm10_0_sum = (pm10_0_sum+pm10_0);
      Firebase.RTDB.setFloat(&fbdo, date+"/PM_10_0_Sum", pm10_0_sum);
    }
    else
    {
      Firebase.RTDB.setFloat(&fbdo, date+"/PM_10_0_Sum", 0);
      pm10_0_sum = 0;
    }

    
    if (Firebase.RTDB.getFloat(&fbdo,"/"+date+"/Count"))
    {
      count = fbdo.floatData();
      count++;
      Firebase.RTDB.setFloat(&fbdo, date+"/Count", count);
      
    }
    else
    {
      Firebase.RTDB.setFloat(&fbdo, date+"/Count", 0);
      count = 0;
    }

    

    

    // Write humidity on the database
    if (Firebase.RTDB.setFloat(&fbdo, date+"/Humidity", h))
    {
      Serial.print("Humidity: ");
      Serial.println(h);

    }
    else
    {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    

    // Write temperature on the database
    if (Firebase.RTDB.setFloat(&fbdo, date+"/Temperature", t))
    {
      Serial.print("Temperature: ");
      Serial.println(t);
    }
    else
    {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }


    // Write CO, TVOC on the database
    if ((Firebase.RTDB.setFloat(&fbdo, date+"/CO", co)))
    {
      Serial.print("CO: ");
      Serial.print(co);
      Serial.println(" ppm");
    }
    else
    {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }



    // Write CO2, TVOC on the database
    if ((Firebase.RTDB.setFloat(&fbdo, date+"/CO2", co2)) && (Firebase.RTDB.setFloat(&fbdo, date+"/TVOC", tvoc)))
    {
      Serial.print("CO2: ");
      Serial.print(co2);
      Serial.println(" ppm");
      
      
      Serial.print("TVOC: ");
      Serial.print(tvoc);
      Serial.println(" ppb");
    }
    else
    {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }


    

    // Write PM1.0, PM2.5, PM10.0 on the database
    if ((Firebase.RTDB.setFloat(&fbdo, date+"/PM_1_0", pm1_0)) && (Firebase.RTDB.setFloat(&fbdo, date+"/PM_2_5", pm2_5)) && (Firebase.RTDB.setFloat(&fbdo, date+"/PM_10_0", pm10_0)))
    {
      Serial.print("PM 1.0 (ug/m3): ");
      Serial.println(pm1_0);
      Serial.print("PM 2.5 (ug/m3): ");
      Serial.println(pm2_5);
      Serial.print("PM 10.0 (ug/m3): ");
      Serial.println(pm10_0);
      Serial.println();
    }
    else
    {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }

  }
  Serial.println("______________________________");
}
