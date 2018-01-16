# A-SYNC: Automatically Synchronized Audience Interaction Device
Git repository for our arduino code. In order to use libraries on Windows they have to go in 
\Documents\Arduino\libraries
For this code, the most important library is the IR library, which includes the header file IRremote.h

# Files
stateGlobals.h
This file includes the state variables and state arrays

AccelState.ino
Processes data off of the accelerometer and translates the data into distinct states.

IRTransmit.ino
Transmits the current state every tenth of a second.

IReceive.ino
Decodes and stores all incoming IR data.

IRrx.ino and IRtx.ino
These files are testing code for IR reception and transmission.

TimerOne.cpp, TimerOne.h
These files control the 16 bit timer, Timer One.

TimerThree.cpp, TimerThree.h
Contrary to the name, these files control the 8 bit timer Timer Two. Timer Three is not available on the ATMEGA328

async.ino
The main file. Includes calls to all other setup and loop code.

keywords.txt
Helper file for TimerOne and TimerThree code.

ledControl.ino
Controls the output of the LEDs and updates the LED state.
