/* Greg 10-31-17

Design Description:
State control is deterministic for the IR Receiver

Trigger:
Loop

Inputs:
PB3 (17), PD5 (11), PD6 (12), PD7 (13), PB0 (14), PB1 (15), PB2 (16)
These inputs are the IR Receiver Pins

Outputs:
RxState

*/

#if !ANATOLE_REC
#include "stateGlobals.h"
//PD5(5), PD6(6), PD7(7), PB0(8), PB2(10), PB3(11), PB4(12)
IRrecv irrecv(11);
IRrecv irrecv2(5);
IRrecv irrecv3(6);
IRrecv irrecv4(7);
IRrecv irrecv5(8);
IRrecv irrecv6(10);
IRrecv irrecv7(12);
decode_results results;


decode_results resultsRx[7];
decode_results resultsRxTemp;

// Storage for the recorded code
int codeType = -1; // The type of code
unsigned long codeValue; // The code value if not raw
unsigned int rawCodes[RAWBUF]; // The durations if raw
int codeLen; // The length of the code
int toggle = 0; // The RC5/6 toggle state
int count1;

// Stores the code for later playback
// Most of this code is just logging
void storeCodeLocal(decode_results *result, int receiverNumber) {

  Serial.println("Receiving");

  codeType = result->decode_type;
  //only use the NEC code type
  if (codeType != NEC){
    return;
  }
  
  resultsRx[receiverNumber-1] = *result;
  //Serial.println(result->value, HEX);
}

unsigned long lastInfoTime(999999999);
void processInputs()
{
  long knownIDs[7] = {0, 0, 0, 0, 0, 0, 0};
  bool skip = false;
  byte knownLEDStates[10] = {0,0,0,0,0,0,0,LEDState,LEDState,LEDState};
  byte knownWheelPositions[10] = {0,0,0,0,0,0,0,wheelPos,wheelPos,wheelPos};
  byte newInfo(0);
  //update RxState
  for (int i=0;i<7;i++)
 {
  //check for duplicate IDs
  for (int a=0;a<i;a++)
  {
    knownIDs[i] = resultsRx[i].value >> 16;
    if (knownIDs[a] == knownIDs[i] || (long)knownIDs[i] == ID)
    {
      skip = true;
      break;
    }
    skip = false;
  }
  //interpret the incoming 24 bits of information as a position along the color wheel
  if (!skip)
  {
    
    for (int i=0;i<7;i++)
    {
      if (resultsRx[i].value > 0)
      {
        //first 8 bits are wheelPos, next 8 are LEDState, next 16 are the ID
        knownLEDStates[i] = (resultsRx[i].value >> 8) & 255;
        knownWheelPositions[i] = (resultsRx[i].value) & 255;
        Serial.println("LED and Wheel Positions");
        Serial.println(knownLEDStates[i]);
        Serial.println(knownWheelPositions[i]);
        newInfo++;
      }
    }
      //codeValue += wheelPos;//this value is the position on the color wheel
 //codeValue+=LEDState << 8;
  //codeValue += ID << 16;
  }
 }
 if (newInfo == 0 && ((millis() - lastInfoTime) < 2000)) return; //if we don't have new information, don't update things.
 lastInfoTime = millis();
 
 //mediate received states and determine how we want to set the global RxState variable.
 byte knownLEDStatesVotes[10] = {1,0,0,0,0,0,0,0,0,0};
 byte knownWheelPositionsVotes[8] = {1,0,0,0,0,0,0,0};
 for (int i=1;i<10;i++)
 {
  for (int a=0;a<i;a++)
  {
    if (knownLEDStates[a] == knownLEDStates[i])
    {
      knownLEDStatesVotes[a]++;
    }
    if (knownWheelPositions[a] > 0 && i < 9 && ((knownWheelPositions[a] >= (knownWheelPositions[i] - 5)) || (knownWheelPositions[a] <= (knownWheelPositions[i] + 5))))
      knownWheelPositionsVotes[a]++;
  }
 }
 //votes are now collected in the beginning positions of the array
 //therefore, we can examine the array to determine who has the highest vote count
 byte popularLEDState = 0;
 byte popularLEDStateCt = 0;
 byte popularwheelPos = wheelPos;
 byte popularwheelPosCt = 0;
 if ((knownLEDStates[7] & 2) == 2) knownLEDStates[7] -= 2;//do not believe everything you tell yourself
 //if ((popularLEDState & 2) == 2) popularLEDState -= 2;//you cannot wave by yourself
 for (int i=0;i<7;i++)
 {
  if (i < 7)
  {
    if ((knownWheelPositionsVotes[i] > popularwheelPosCt) && (knownWheelPositions[i] != 0)&& (knownWheelPositions[i] != 85)&& (knownWheelPositions[i] != 170))
    {
      popularwheelPos = knownWheelPositions[i];
      popularwheelPosCt = knownWheelPositionsVotes[i];
    }
  }
  if ((knownLEDStatesVotes[i] >= popularLEDStateCt) && (knownLEDStates[i] != 0))
  {
    popularLEDStateCt = knownLEDStatesVotes[i];
    popularLEDState = knownLEDStates[i];
  }
 }

 //Finally, update the wheel position and LED state
 unsigned int RxStateTemp = 0;
 RxStateTemp += popularwheelPos;
 unsigned int p = popularLEDState << 8;
 //check if we want to update LEDState
 //unsigned int updateLEDV(0);
 //for (int i=0;i<7;i++) if (knownLEDStates[i] > 0) updateLEDV++;
 //if (updateLEDV >= 3)
  RxStateTemp += p;

 //update the global state
   Serial.println("RxStateTemp popularwheelPos popularLEDState");
  Serial.print(RxStateTemp);
  Serial.print("-");
  Serial.print(popularwheelPos);
  Serial.print("-");
  Serial.println(popularLEDState);
  RxState = RxStateTemp;

 //adjust the wheel position based on received data / RxState
  //if in the same LEDState, synchronize wheelPos
  if ((popularwheelPos != 0)&&(popularwheelPos != 85) && (popularwheelPos != 170))
   { wheelPos = (popularwheelPos);
   Serial.print("Wheelpos: ");
 Serial.println(wheelPos);
   }
}

//this is Anatole's receiver code
void runReceiver(IRrecv irrecvIn, int recNum)
{
  irrecvIn.enableIRIn(); // Start the receiver
  for(int i =0; i<10; i++){
    delay(10);
    if (irrecvIn.decode(&results)) {
      storeCodeLocal(&results, recNum);
      irrecvIn.resume();
    }
  }
  cli();
  TIMER_DISABLE_INTR;
  TIMER_RESET;
  sei();
}

void IReceiveControl(int thingToReceiveOn)
{ 
  switch (thingToReceiveOn)
  {
    case 1:
    runReceiver(irrecv, 1); break;
    case 2:
    runReceiver(irrecv2, 2);
    break;
    case 3:
    runReceiver(irrecv3, 3);
    break;
    case 4:
    runReceiver(irrecv4, 4);
    break;
    case 5:
    runReceiver(irrecv5, 5);
    break;
    case 6:
    runReceiver(irrecv6, 6);
    break;
    case 7:
    runReceiver(irrecv7, 7);
    processInputs();
     //zero out communication information
 for (int i=0;i<7;i++) resultsRx[i].value = 0;
    break;
  }
}

#endif
