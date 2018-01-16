// Greg 10-31-17
// This file controls the LED outputs

/*
Design Description:
LED outputs are deterministic, with a single set of input states corresponding to one output state.

Trigger:
Interrupt every 200ms with Timer1

Inputs:
RxState, AccelState
These inputs are global and stored in stateGlobals.h

Outputs:
Neopixel strip setting
Update LEDState

*/
#include "stateGlobals.h"

#define REDTIME 15

//length, pin and type are part of the constructor
Adafruit_NeoPixel strip = Adafruit_NeoPixel(1, 9, NEO_GRB + NEO_KHZ800);
int isLitCount = 0;

void ledControlSetup()
{
	//Timer1.initialize(tenthOfSecond*5);
	//Timer1.attachInterrupt(ledControlInterrupt);
	
	//TODO
  strip.begin();
  strip.clear();
  strip.show();
  
}

uint8_t updateColorRotation(uint8_t RECEIVING)
{
  return RECEIVING + 8;
}

uint32_t Wheel(byte WheelPos, int valuefactor) {
  if(WheelPos < 85) {
    return strip.Color((WheelPos * 3)/valuefactor, (255 - WheelPos * 3)/valuefactor, 0);
  }
  else if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color((255 - WheelPos * 3)/valuefactor, 0, (WheelPos * 3)/valuefactor);
  }
  else {
    WheelPos -= 170;
    return strip.Color(0, (WheelPos * 3)/valuefactor, (255 - WheelPos * 3)/valuefactor);
  }
}
unsigned int callCt;
unsigned long otherWaveTime(999999999);

//Supposed to handle all possible states and all possible transitions
//Possible states: THE wave (up and down), jumping (regular flashing), beating the thing (regular flashing), slow wave above head
void ledControlInterrupt()
{
  //Increase wheel position: color rotation speed
  if (callCt > 5){
    callCt = 0;
    wheelPos++;
  }
  callCt++;

  //Everything for setting output is below here
  #if SERIAL_ENABLE
	Serial.print("newLEDInterrupt");
  Serial.println(AccelState);
  #endif
  uint32_t LEDStateTemp = 0;
bool waving = false;
//if someone else is getting ready to wave, give us a time limit to wave.
 if ((RxState & 512) == 512)
 {
   otherWaveTime = millis();
   Serial.println("OtherWaveTime set");
 }
 if ((AccelState & 2) == 2 && (millis() - otherWaveTime) < 4000) //4 seconds since we paused (or were currently paused)
 {
  Serial.println("Wave.");
    strip.clear();
  strip.setPixelColor(0, 200, 200, 200);
  strip.show();
  LEDStateTemp+=2;
  waving = true;
 }
 else if (((AccelState & 2) == 2) || ((AccelState & 1) == 1)){
   Serial.print("AccelState_2 RxState "); Serial.println(RxState);
   LEDStateTemp+=2;
 }
	if ((AccelState & 4) == 4 && !waving )//new beat => flash flip!
	{
		Serial.println("flash flip");
		strip.clear();
    wheelPos+=85;//move the color forward a lot since we're beating
    strip.setPixelColor(0,Wheel(wheelPos, 1));
    //strip.setPixelColor(0, 255/brightness, RECEIVING/brightness, RECEIVING/brightness);
    //strip.setPixelColor(#LED, r, g, b);
    strip.show();
    isLitCount = REDTIME;
    LEDStateTemp+=4;
	}
 else if (((AccelState & 16) == 16 || ((AccelState&8)==8))&& isLitCount <= 0 && !waving) //static movement
 {
  Serial.println("slow or static movement");
  strip.clear();
  strip.setPixelColor(0,Wheel(wheelPos, 1));
  //strip.setPixelColor(0, RECEIVING/brightness, RECEIVING/brightness, 255/brightness);
  strip.show();
  LEDStateTemp+=16;
 }
 else if ((AccelState & 32) == 32 && isLitCount <= 0 && !waving) // fast movement
 {
  Serial.println("fast movement");
  strip.clear();
  strip.setPixelColor(0,Wheel(wheelPos, 1));
  //strip.setPixelColor(0, RECEIVING/brightness, RECEIVING/brightness, 255/brightness);
  strip.show();
  LEDStateTemp+=32;
 }

 
 --isLitCount;
 if (isLitCount < 0) isLitCount = 0;

 //transmit our state if our state has changed
 if (LEDState != LEDStateTemp) 
 {
  LEDState = LEDStateTemp;
  IRTransmitControl();
 }
 LEDState = LEDStateTemp;
}

/*
 * RxState:
 * LSB indicates movement going DOWN
 * LSB+1 indicates movement going UP
 * LSB+2 indicates red flash
 * LSB+3 indicates fast movement
 * LSB+4 indicates slow/static movement
 * 
 */

/*
// Code for testing
void setup()
{
	ledControlSetup();
}

void loop()
{}

*/
