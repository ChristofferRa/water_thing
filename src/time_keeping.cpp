
#include "time_keeping.h"

/*********************************
**** Time keeping functions   ****
**********************************/

#include <time.h>
#include <Arduino.h>

void timeSetup(){
    //***************************
    //---  Set up NTP Servers --- 
    //***************************
    
    // Configure NTP servers
    configTime(0, 0, "se.pool.ntp.org", "time.google.com");

    // Set timezone to UTC+1 with DST
    setenv("TZ", "CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00", 1);
    tzset();

}