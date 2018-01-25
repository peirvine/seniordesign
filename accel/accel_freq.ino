#include <Wire.h>
#include <SPI.h>
#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>

Adafruit_LIS3DH lis = Adafruit_LIS3DH();

void setup(void) {
#ifndef ESP8266
  while (!Serial);     // will pause Zero, Leonardo, etc until serial console opens
#endif

  Serial.begin(9600);
  if (! lis.begin(0x18)) {   // change this to 0x19 for alternative i2c address
    Serial.println("Couldnt start");
    while (1);
  }
  
  lis.setRange(LIS3DH_RANGE_8_G);   // 2, 4, 8 or 16 G!
}

unsigned long time=0;
float prevmax=0;
float prevmin=0;
float max=0;
float min=0;
unsigned long freq=0;
void loop() {
  sensors_event_t event; 
  lis.getEvent(&event);
  if(time==1000){
    //Serial.print(min);
    //Serial.print(",");
    //Serial.print(max);
    //Serial.println();
    prevmax=max;
    prevmin=min;
    max=0;
    min=0;
    time=0;
  }
  if(event.acceleration.y>(prevmax-3.0)){
    Serial.println(freq);
    freq=0;
    prevmax=500;
  }
  if(event.acceleration.y>max)
    max=event.acceleration.y;
  if(event.acceleration.y<min)
    min=event.acceleration.y;
  time++;
  freq++;
}
