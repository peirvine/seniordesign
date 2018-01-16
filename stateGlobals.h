#ifndef STATE_GLOBALS_H
#define STATE_GLOBALS_H

#define tenthOfSecond 100000

unsigned int RxState(0), AccelState(0);
byte wheelPos(0);
byte LEDState(0);

long ID(0); 

#endif

/*
 * RxState:
 * first 8 bits are wheelPos, next 8 are LEDState, next 16 are the ID
 * 
 * AccelState:
 * LSB indicates a wave movement going DOWN [ACCURATE if pattern is 11111111122222222222] -> for faster waves, will be 1111111118.
 * LSB+1 indicates a 'wave like' movement going UP [ACCURATE if pattern 222222222222222111111] -> for faster waves, will be 222222222228.
 * LSB+2 indicates a beating movement @ the edge (aka about to turn around and come back) [VERY ACCURATE]
 * LSB+3 is set if the velocity is between 0 and  [INACCURATE/doesn't make sense]
 * LSB+4 is set if the velocity is between 8 and 12 [ACCURATE]
 * LSB+5 is set if the velocity is above 13 [ACCURATE]
 * 
 * LEDState:
 * Same as RxState, we're just going to Xmit it instead of receiving it
 * LSB indicates movement going DOWN
 * LSB+1 indicates movement going UP
 * LSB+2 indicates red flash
 * LSB+3 indicates fast movement
 * LSB+4 indicates slow/static movement
 */
