#include <SPI.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LIS3DH.h>

Adafruit_LIS3DH accel = Adafruit_LIS3DH();

sensors_event_t event;
unsigned long start_time; //used as the reference for time 0 by storing millis() value at start of data collection in this variable

void setup() {
  accel.begin(0x18);
  accel.setRange(LIS3DH_RANGE_2_G);
  Serial.begin(9600);
  while(!Serial);  //wait until Serial Monitor is opened to continue
  start_time=millis();
}

void loop() {
  accel.getEvent(&event);
  Serial.print(millis()-start_time);
  Serial.print(",");
  Serial.print(event.acceleration.x);
  Serial.print(",");
  Serial.print(event.acceleration.y);
  Serial.print(",");
  Serial.print(event.acceleration.z);
  Serial.println();
}
