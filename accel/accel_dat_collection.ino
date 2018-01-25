#include <Wire.h>
#include <SPI.h>
#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_NeoPixel.h>

#define NUM_SAMP 10
#define THRES 0.25

float avg_x,avg_y,avg_z;
float prev_x,prev_y,prev_z;
sensors_event_t event;

Adafruit_LIS3DH lis = Adafruit_LIS3DH();
Adafruit_NeoPixel strip = Adafruit_NeoPixel(8, 4, NEO_GRB + NEO_KHZ800);

void setAllPixels(Adafruit_NeoPixel* pixels,uint8_t r,uint8_t g,uint8_t b){
  for(int i=0; i< pixels->numPixels(); i++)
    pixels->setPixelColor(i,r,g,b);
  pixels->show();    
}

void setup(void) {
  while (!Serial); 

  Serial.begin(9600);
  if (! lis.begin(0x18)) {   // change this to 0x19 for alternative i2c address
    Serial.println("Couldnt start");
    while (1);
  }
  
  lis.setRange(LIS3DH_RANGE_4_G);   // 2, 4, 8 or 16 G!
  strip.begin();
  strip.show();
  
  for(int i=0;i<NUM_SAMP;i++){
    lis.getEvent(&event);
    prev_x+=event.acceleration.x;
    prev_y+=event.acceleration.y;
    prev_z+=event.acceleration.z;
  }
  prev_x/=NUM_SAMP;
  prev_y/=NUM_SAMP;
  prev_z/=NUM_SAMP;
}

unsigned long time=0;
boolean rising=false;
unsigned long period=0;
unsigned long prev_period=0;
unsigned long avg_period=0;
unsigned long start_time=0;

void loop() {
  avg_x=0;
  avg_y=0;
  avg_z=0;
  
  for(int i=0;i<NUM_SAMP;i++){
    lis.getEvent(&event);
    avg_x+=event.acceleration.x;
    avg_y+=event.acceleration.y;
    avg_z+=event.acceleration.z;
  }
  avg_x/=NUM_SAMP;
  avg_y/=NUM_SAMP;
  avg_z/=NUM_SAMP;
  if(avg_x<prev_x+THRES && avg_x>prev_x-THRES)
    avg_x=prev_x;
  if(avg_y<prev_y+THRES && avg_y>prev_y-THRES)
    avg_y=prev_y;
  if(avg_z<prev_z+THRES && avg_z>prev_z-THRES)
    avg_z=prev_z;    
    
  Serial.print(time);
  Serial.print(",");
  Serial.print(avg_x);
  Serial.print(",");
  Serial.print(avg_y); 
  Serial.print(",");
  Serial.print(avg_z); 
//  Serial.print(",");
//  if(avg_x<prev_x)
//    Serial.print("0,");
//  else
//    Serial.print("1,");
//  if(avg_y<prev_y){
//    strip.clear();
//    strip.show();
//    rising=false;
    //Serial.print("0,");
//  }
//  else{
//    if(!rising){
//      prev_period=period;
//      period=time-start_time;
//      start_time=time;
//      avg_period=(prev_period+period)/2;
//      if(avg_period>4)
//        setAllPixels(&strip,64,0,0);
//    }
//    rising=true;
    //Serial.print("1,");
//  }
//  Serial.print(avg_period);
//  if(avg_z<prev_z)
//    Serial.print("0");
//  else
//    Serial.print("1");
  Serial.println();
  time+=1;
  prev_x=avg_x;
  prev_y=avg_y;
  prev_z=avg_z;
}
