#include <MD_MAX72xx.h>
#include <SPI.h>

#define  delay_t  50  // in milliseconds

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 1

//Hardware SPI Arduino UNO
// CLK Pin  > 13 SCK
// Data Pin > 11 MOSI

#define CS_PIN    10

// Hardware SPI connection
MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

byte cross[8] = {0x81,0x42,0x24,0x18,0x18,0x24,0x42,0x81};
byte check[8] = {0x00,0x00,0x80,0x40,0x20,0x11,0x0A,0x04};


void setup() {  
  mx.begin();
  mx.control(MD_MAX72XX::INTENSITY, 2);
  mx.clear();
  }

void loop() {

  for (int i = 0; i <= 7; i++) {
    mx.setRow(0, 0, i, check[i]);
  }

  delay(2000);
  mx.clear();

  for (int i = 0; i <= 7; i++) {
    mx.setRow(0, 0, i, cross[i]);
  }
  delay(2000);
  mx.clear();
}
