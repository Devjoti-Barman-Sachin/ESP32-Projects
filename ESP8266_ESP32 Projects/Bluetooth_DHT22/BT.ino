#include <DHT.h>
#include <SoftwareSerial.h>

#define DHTPIN D3      // Pin connected to the DHT11 sensor
#define DHTTYPE DHT11   // DHT11 sensor type (change to DHT22 if using DHT22)

DHT dht(DHTPIN, DHTTYPE);
SoftwareSerial bluetoothSerial(D1, D2); // RX, TX pins for HC-05/HC-06

void setup() {
  Serial.begin(9600);
  bluetoothSerial.begin(9600); // Bluetooth module baud rate
  dht.begin();
}

void loop() {
  delay(2000); // Wait for 2 seconds between readings

  // Read temperature and humidity from the DHT11 sensor
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();



  // Print temperature and humidity to the serial monitor
  bluetoothSerial.print("Temperature: ");
  bluetoothSerial.print(temperature);
  bluetoothSerial.print(" °C\t");

  
}
