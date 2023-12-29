#include <Wire.h>
#include <Adafruit_MLX90614.h>

Adafruit_MLX90614 mlx = Adafruit_MLX90614();

void setup() {
  Serial.begin(115200);
  mlx.begin(); // Initialize the MLX90614 sensor

  // Initialize I2C communication
  Wire.begin();
}

void loop() {
  // Read temperature values
  float ambientTemp = mlx.readAmbientTempC();
  float objectTemp = mlx.readObjectTempC();

  // Print temperature readings
  Serial.print("Ambient Temperature: ");
  Serial.print(ambientTemp);
  Serial.println(" °C");

  Serial.print("Object Temperature: ");
  Serial.print(objectTemp);
  Serial.println(" °C");

  delay(1000); // Delay for 1 second
}
