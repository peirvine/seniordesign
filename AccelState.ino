// Greg 10-31-17
// AccelState.ino
// This file receives and updates the current receive state

/*
  Design Description:
  State control is deterministic for the Accelerometer

  Trigger:
  Loop, but should not be run more than 20 times / second (pretty unlikely, but I'm throwing this out there anyway)

  Inputs:
  PC5 (SCL) (28); PC4 (SDA) (27)

  Outputs:
  AccelState

*/


#include "stateGlobals.h"

Adafruit_LIS3DH accel = Adafruit_LIS3DH();

#define HISTORY_SIZE 20 //minimum of 10
unsigned long start_time;
//int accelStateIntermediateHistory[30]; //the thing that will actually determine an accelState?
sensors_event_t lastValue;
float floatChangeAccelHistory[HISTORY_SIZE]; //beat-based acceleration: difference between last value and this value -> acceleration
float floatAbsAccelHistory[HISTORY_SIZE]; //a calculation of movement up and down only: used for the wave
unsigned int accelHistoryIndex(0);
sensors_event_t lastEvent;

unsigned long beatTimeHistory[8] = {0, 0, 0, 0, 0, 0, 0, 0};
unsigned int beatTimeHistoryIndex(0);

//for the wave, have a pausing time
unsigned long pausingTime(999999999);

void accelControlSetup()
{
  //pinMode(28, INPUT);
  //pinMode(27, INPUT);

  accel.begin(0x18);
  accel.setRange(LIS3DH_RANGE_2_G);
  Serial.begin(9600);
  start_time = millis();
}

//returns the jerk between two measurements
float deltaAccel(sensors_event_t* a, sensors_event_t* b)
{
  float xDif = a->acceleration.x - b->acceleration.x;
  float yDif = a->acceleration.y - b->acceleration.y;
  float zDif = a->acceleration.z - b->acceleration.z;

  return sqrt((zDif * zDif) + yDif * yDif + xDif * xDif);
}

//returns an acceleration value
float absoluteAccel(sensors_event_t* a)
{
  return sqrt((a->acceleration.x * a->acceleration.x) + (a->acceleration.y * a->acceleration.y) + a->acceleration.z * a->acceleration.z);
}

#define ASHSize 50
int AccelStateHistory[ASHSize];
unsigned int AccelStateHistoryNext(0);

//returns the velocity in accelstatehistory
float velocity()
{
  int i = (accelHistoryIndex + 1) % HISTORY_SIZE;
  float v = 0.0;
  for (int c = 0; c < HISTORY_SIZE; c++)
  {
    //assume a uniform amount of time, time value = 1
    v += float(floatAbsAccelHistory[i]);
    if (++i >= HISTORY_SIZE) i = 0;
  }
  return v / 20.0;
}

//update the AccelState global variable
void updateAccelState(unsigned int accelHistoryIndex)
{
  int AccelStateTemp = 0;
  
  //globals used for stuff
  int pausingVotes = 0;//not moving much
  bool falling = true;//less gravity
  int i = accelHistoryIndex;
  for (int c = 0; c < HISTORY_SIZE; c++)
  {
    if (floatAbsAccelHistory[i] < 13 && floatAbsAccelHistory[i] > 8)
    {
      pausingVotes++;
    }
    i--;
    if (i < 0) i = HISTORY_SIZE - 1;
  }

  //LSB indicates there has been a long pause in movement (as though the wave is about to start)
  if (pausingVotes > 15)
  {
    pausingTime = millis();
    AccelStateTemp |= 1;
  }

  //LSB+1 bit indicates it's been less than 3 seconds since we stopped moving, so we could be doing the wave.
  if ((millis() - pausingTime) < 3000 && (AccelStateTemp == 0))
    AccelStateTemp |= 2;

  //LSB+6 indicates a very large and long acceleration change
  if (floatChangeAccelHistory[accelHistoryIndex] > 3 && floatChangeAccelHistory[accelHistoryIndex] < 9 &&
      floatChangeAccelHistory[(accelHistoryIndex - 1) % HISTORY_SIZE] > 3 && floatChangeAccelHistory[(accelHistoryIndex - 1) % HISTORY_SIZE] < 9 &&
    floatChangeAccelHistory[(accelHistoryIndex - 2) % HISTORY_SIZE] > 3 && floatChangeAccelHistory[(accelHistoryIndex - 2) % HISTORY_SIZE] < 9 &&
    floatChangeAccelHistory[(accelHistoryIndex - 3) % HISTORY_SIZE] > 3 && floatChangeAccelHistory[(accelHistoryIndex - 3) % HISTORY_SIZE] < 9 &&
      floatChangeAccelHistory[(accelHistoryIndex - 4) % HISTORY_SIZE] > 3 && floatChangeAccelHistory[(accelHistoryIndex - 4) % HISTORY_SIZE] < 9
    
    )
  {
#if SERIAL_ENABLE
    Serial.println(floatChangeAccelHistory[accelHistoryIndex]);
#endif
    //add this thing to the beatTimeHistory
    // if it's been longer than 300ms since we registered a beat, add it to beatTimeHistory
    unsigned long t = millis();
    if (t - beatTimeHistory[beatTimeHistoryIndex] > 500)
    {
      AccelStateTemp |= 64;
    }
  }

  //LSB+2 indicates a temporarily very large acceleration
  if (floatChangeAccelHistory[accelHistoryIndex] > 3 && floatChangeAccelHistory[accelHistoryIndex] < 9 &&
  floatChangeAccelHistory[(accelHistoryIndex - 1) % HISTORY_SIZE] > 1 && floatChangeAccelHistory[(accelHistoryIndex - 1) % HISTORY_SIZE] < 9
  )
  {
#if SERIAL_ENABLE
    Serial.println(floatChangeAccelHistory[accelHistoryIndex]);
#endif
    //add this thing to the beatTimeHistory
    // if it's been longer than 300ms since we registered a beat, add it to beatTimeHistory
    unsigned long t = millis();
    if (t - beatTimeHistory[beatTimeHistoryIndex] > 350)
    {
      beatTimeHistoryIndex++;
      if (beatTimeHistoryIndex > 7) beatTimeHistoryIndex = 0;
      beatTimeHistory[beatTimeHistoryIndex] = t;
      AccelStateTemp |= 4;
    }
  }

  //LSB+3 is the velocity (calculated as the integral of the last 20 measurements)
  float v = velocity();
  if (v <= 8.0)
  {
    AccelStateTemp |= 8;
  }
  else if (v <= 13.0 && v > 8.0)
  {
    AccelStateTemp |= 16;
  }
  else if (v > 13.0) //if it's a large velocity
  {
    AccelStateTemp |= 32;
  }
  AccelState = AccelStateTemp;
}

// Read in the accelerometer state and transform it to values from 0-9
void accelControl()
{
  sensors_event_t averageEvent;
  sensors_event_t event;

  //accumulate all acceleration values
  accel.getEvent(&averageEvent);
  //TODO: floatHeightHistory needs to be implemented and tested and the new data put into a waveTest and looked at.
  for (int i = 0; i < 3; i++)
  {
    accel.getEvent(&event);
    averageEvent.acceleration.x += event.acceleration.x;
    averageEvent.acceleration.y += event.acceleration.y;
    averageEvent.acceleration.z += event.acceleration.z;
  }
  //average all of the acceleration values
  averageEvent.acceleration.x /= 3;
  averageEvent.acceleration.y /= 3;
  averageEvent.acceleration.z /= 3;

  //update the acceleration history for future reference
  if (lastEvent.version == NULL || lastEvent.version == 0)
  {
    floatChangeAccelHistory[accelHistoryIndex] = 0;
    floatAbsAccelHistory[accelHistoryIndex] = 0;
  }
  else
  {
    floatChangeAccelHistory[accelHistoryIndex] = deltaAccel(&averageEvent, &lastEvent);
    floatAbsAccelHistory[accelHistoryIndex] = absoluteAccel(&averageEvent);
  }

  lastEvent = averageEvent;

  updateAccelState(accelHistoryIndex);

  //verify that the history is being populated correctly
  //Serial.print(millis()-start_time);
  //Serial.print(",");
  //Serial.print(floatChangeAccelHistory[accelHistoryIndex]);
  //Serial.print("fABSAccelHistory");
  //Serial.println(floatAbsAccelHistory[accelHistoryIndex]);
  //Serial.print(",");
  //Serial.print(AccelState);
  //Serial.println();

  //update the index for subsequent calls to this function
  if (accelHistoryIndex > HISTORY_SIZE) {
    accelHistoryIndex = 0;
  }
  else accelHistoryIndex++;
}
/*
  // Code for testing
  void setup()
  {
	ledControlSetup();
  }

  void loop()
  {}

*/
