#ifndef MULTITASKER_H
#define MULTITASKER_H

/*
Multitasker class for easy multitasking in arduino projects
By Christoffer Rappmann, christoffer.rappmann@gmail.com

It provides a class multiTasker that is initilized with a run intervall in milliseconds.
In the arduino loop()-function a if statement can be used with the function .isTime() that 
returns true if the time intervall has been passed, if not it remains false.

multiTasker Class:
    Purpose: 
        Provides a mechanism for multitasking by checking if a specified time interval has elapsed.
    Private Variables:
        lastMillis: Stores the time of the last multitasking check.
        interval: Stores the specified time interval in milliseconds.
    Public Methods:
        multiTasker(unsigned long _interval): Constructor to initialize the multiTasker object with a specified time interval.
        bool isTime(): Checks whether the specified time interval has elapsed since the last check. Returns true if the interval has passed and resets the counting; otherwise, returns false.

*/

#include <Arduino.h>

class multiTasker {
    /* Multitasking class, checks wether a predetermined (during initilizing a object) time in miliseconds
    has elapsed since last time it returned true. 

    For each time the "isTime()" functions returns true the lastMillis is set to the currentMillis which is
    the current system time in milliseconds using the built in millis() function. 

    If more or equal time that was defined in the interval variable have elapsed since last it will return true 
    and reset the counting, if not it will return false.
    */
private:
    unsigned long lastMillis;
    unsigned long interval;

public:
    multiTasker(unsigned long _interval) {
        interval = _interval;
        lastMillis = 0;
    }

    bool isTime() {
        unsigned long currentMillis = millis();
        if (currentMillis - lastMillis >= interval) {
            lastMillis = currentMillis;
            return true;
        } else {
            return false;
        }
    }
};

#endif
