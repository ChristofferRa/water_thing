/*
hardware_functions
By Christoffer Rappmann, christoffer.rappmann@gmail.com 
*/

#include "hardware_functions.h"
#include <Arduino.h>

// used pins for switches, not in code atm.
/*
const int buttonSW1 = 15;
const int buttonSW2 = 2;
*/

// Valve
RTC_DATA_ATTR bool valveState = true; // Open or closed, retain after sleep
