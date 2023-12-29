#define TINY_GSM_MODEM_SIM800
#define TINY_GSM_RX_BUFFER 256

#include <TinyGsmClient.h> 
#include <ArduinoHttpClient.h> 
#include <DHT.h>
#include <SoftwareSerial.h>

#define rxPin 3
#define txPin 2

const int trigPin = 10;
const int echoPin = 8;
const int gasPin = A0;
const int BinSize = 135; //(cm)
const int SafetyLevel = 0; //(%)



SoftwareSerial sim800(txPin, rxPin);

#define DHTPIN 6

DHT dht(DHTPIN, DHT22);

const char FIREBASE_HOST[]  = "digidata-cb297-default-rtdb.firebaseio.com";
const String FIREBASE_AUTH  = "QJt3kaDiMbh7PQzCvZkDbFovE0qOH3nvLrib6vbP";
const String FIREBASE_PATH  = "/Sensor/SIR01/";
const int SSL_PORT          = 443;

char apn[]  = "www";//airtel ->"airtelgprs.com"
char user[] = "";
char pass[] = "";


TinyGsm modem(sim800);

TinyGsmClientSecure gsm_client_secure_modem(modem, 0);
HttpClient http_client = HttpClient(gsm_client_secure_modem, FIREBASE_HOST, SSL_PORT);

unsigned long previousMillis = 0;


void setup()
{

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  Serial.begin(115200);
  dht.begin();
  Serial.println("device serial initialize");

  sim800.begin(9600);
  Serial.println("SIM800L serial initialize");

  Serial.println("Initializing modem...");
  modem.restart();
  String modemInfo = modem.getModemInfo();
  Serial.print("Modem: ");
  Serial.println(modemInfo);

  http_client.setHttpResponseTimeout(10 * 1000); //^0 secs timeout
}

void loop()
{

  //--------> Here we have modified the code...!
  Serial.print(F("Connecting to "));
  Serial.print(apn);
  Serial.println(" ");
  if (modem.gprsConnect(apn, user, pass))
  {
    Serial.println(" gprs connected!");
    delay(10);
  }

  http_client.connect(FIREBASE_HOST, SSL_PORT);

  while (true) {
    sens_loop();
    if (!http_client.connected())
    {
      Serial.println();
      http_client.stop();// Shutdown
      Serial.println("HTTP  not connect");
      break;
    }
    else
    {
      sens_loop();
      delayMicroseconds(30000000);
      
    }
    

  }

}



void PostToFirebase(const char* method, const String & path , const String & data, HttpClient* http)
{
  String response;
  int statusCode = 0;
  http->connectionKeepAlive(); // Currently, this is needed for HTTPS

  String url;
  if (path[0] != '/')
  {
    url = "/";
  }
  url += path + ".json";
  url += "?auth=" + FIREBASE_AUTH;
  Serial.print("POST:");
  Serial.println(url);
  Serial.print("Data:");
  Serial.println(data);

  String contentType = "application/json";
  http->put(url, contentType, data);

  statusCode = http->responseStatusCode();
  Serial.print("Status code: ");
  Serial.println(statusCode);
  response = http->responseBody();
  Serial.print("Response: ");
  Serial.println(response);


}

void sens_loop()
{
  
  //------->This section is for only sonar
  int howFilled;
  
  long duration;
  
  int distance;
  
  

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;
  
  howFilled = ( 100 - (100*distance)/BinSize )+ SafetyLevel;
  
  
  String emptySpace = String(howFilled);
  
  Serial.println("--------------------------------------------------------------------");
  Serial.print("Distance : ");
  Serial.println(distance);
  Serial.print("Empty space : ");
  Serial.println(emptySpace);

  String gas = String(analogRead(gasPin));
  String h = String(dht.readHumidity(), 2);
  String t = String(dht.readTemperature(), 2);
 // String id="SIR-01";
  delay(100);

  Serial.print("Temperature = ");
  Serial.print(t);
  Serial.println(" °C");
  Serial.print("Humidity = ");
  Serial.print(h);
  Serial.println(" %");
  Serial.print("Gas:");
  Serial.println(gas);

  String Data = "{";
  Data += "\"humid\":" + h + ",";
  Data += "\"temp\":" + t + ",";
  Data += "\"gas\":" + gas + ",";
  Data += "\"empty\":" + emptySpace + "";
  Data += "}";

  PostToFirebase("PATCH", FIREBASE_PATH, Data, &http_client);

  


}
