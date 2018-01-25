#include <SPI.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LIS3DH.h>
#include <Adafruit_NeoPixel.h>

Adafruit_LIS3DH lis = Adafruit_LIS3DH();
Adafruit_NeoPixel strip = Adafruit_NeoPixel(8, 4, NEO_GRB + NEO_KHZ800);

void setup() {
  lis.begin(0x18);
  lis.setRange(LIS3DH_RANGE_2_G);
  strip.begin();
  strip.show();
}

void loop() {
  lis.read();
  strip.clear();
  for(int i=0;i<constrain(map(lis.z,-32768/2,32767/2,0,9),0,8);i++){
    strip.setPixelColor(i,64,0,0);
  }
  strip.show();
  //delay(20);
}
