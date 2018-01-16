//Greg's code
//The main file for ASync
//you'll need the following library: IRremote by sherriff
//the LIS3DH library; the adafruit universal sensor library
#include "stateGlobals.h"
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_NeoPixel.h>
#include <IRremote.h>

//options
#define SERIAL_ENABLE 0
#define ANATOLE_REC 0


/*
 * IRrecord: record and play back IR signals as a minimal 
 * An IR detector/demodulator must be connected to the input RECV_PIN.
 * An IR LED must be connected to the output PWM pin 3.
 * A button must be connected to the input BUTTON_PIN; this is the
 * send button.
 * A visible LED can be connected to STATUS_PIN to provide status.
 *
 * The logic is:
 * If the button is pressed, send the IR code.
 * If an IR code is received, record it.
 *
 * Version 0.11 September, 2009
 * Copyright 2009 Ken Shirriff
 * http://arcfn.com
 */


//***********************************************************************//
void setup() {
  accelControlSetup();
  ledControlSetup();
  randomSeed(analogRead(A0));
  ID = random(65535);//16 bits

  Serial.begin(9600);
}

void loop() {
  for (int i=1;i<8;i++)
  {
    accelControl();
    ledControlInterrupt();
    #if!ANATOLE_REC
    IReceiveControl(i);
    #endif
  }
  delay(random(10, 50));//randomly delay a bit to decrease chances of devices becoming 'too' in sync
  IRTransmitControl();
}
