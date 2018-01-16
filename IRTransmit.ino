// Greg 10-31-17
// This file controls updating and transmitting the current state

/*
Design Description:
State control is deterministic for the IR transmitter

Trigger:
Loop

Inputs:
LEDState 

Outputs:
PD3 (5)

*/

#include "stateGlobals.h"

IRsend irsend;

//digitalWrite( 6,1/0 );
//Supposed to handle all possible states and all possible transitions
// An alternative approach is a logic based one
void IRTransmitControl()
{
  Serial.println("Sending");
	// find the value to transmit 
	//first 8 bits are the wheelPos, then next 8 are the LEDState, then next 16 are ID
	unsigned long codeValue = 0;
	codeValue += wheelPos;//this value is the position on the color wheel
 Serial.print(codeValue);
 Serial.print(" ");
 LEDState |= 2;
 long LEDStateL = LEDState << 8;
 
 codeValue+=LEDStateL;
 Serial.print(codeValue);
 Serial.print(" ");
  codeValue += ID << 16;

 Serial.println(codeValue);
  
	
	//set the codeLen
	int codeLen = 32;
	
	// transmitc
  cli();
  TIMER_DISABLE_INTR;
  TIMER_RESET;
  sei();
	irsend.sendNEC(codeValue, codeLen);
  cli();
  TIMER_DISABLE_INTR;
  TIMER_RESET;
  sei();
}
