#ifndef TIME_KEEPING_H
#define TIME_KEEPING_H

/*
Time keeping functions

By Christoffer Rappmann, christoffer.rappmann@gmail.com

Contains classes and functions for time management, including setting up time via NTP, managing target times, and calculating time differences.

timeKeeper:
    This class provides functionality for managing target times and calculating time differences. 
    It allows users to specify a target time during initialization and provides methods to determine the time until the target time, increment the target time by a specified number of days, and retrieve the target time as a string. 

    Key functions include:
        - timeUntil() calculates the time until the target time.
        - incrementDays() adds a specified number of days to the target time.
        - timeString() returns the target time as a formatted string.

Functionality includes handling time zones, daylight saving time adjustments, and automatic adjustment of incomplete target time specifications.

Functions:
    - timeSetup() initializes NTP servers and sets up the time zone for the device.

Structures:
    - tm: Represents a date and time broken down into its components (year, month, day, hour, minute, second). Used for specifying target times.

Usage:
    1. Initialize the time setup using timeSetup() to configure NTP servers and time zone.
    2. Create an instance of the timeKeeper class, specifying the target time during initialization.
    3. Utilize timeKeeper methods to manage and calculate time differences as needed.

*/

#include <time.h>
#include <Arduino.h>

void timeSetup();

class timeKeeper {
    /*
    This class provides functionality for managing target times and calculating time differences. 
    It allows users to specify a target time during initialization and provides methods to determine the time until the target time, increment the target time by a specified number of days, and retrieve the target time as a string. 

    Key functions include:
        - timeUntil() calculates the time until the target time.
        - incrementDays() adds a specified number of days to the target time.
        - timeString() returns the target time as a formatted string.

    */
private:
    //tmElements_t targetTime;
    tm targetTime;

public:
    timeKeeper(int hour, int minute) {
        // Init timeKeeper class
        // Get current time
        time_t currentTime = getCurrentTime();
        
        localtime_r(&currentTime, &targetTime);

        // Set target hour and minute
        targetTime.tm_hour = hour;
        targetTime.tm_min = minute;
        targetTime.tm_sec = 0;  // Reset seconds to zero
    }

    static time_t getCurrentTime() {
        // Static method to get the current time

        // Get current time
        time_t now = time(nullptr);
        return now;
    }

    String timeString() {
        // Method to retrieve the target time as a formatted string

       char buffer[80]; // Buffer to store formatted time string

        // Format timeinfo structure into a string using strftime
        strftime(buffer, sizeof(buffer), "%A, %B %d %Y %H:%M:%S zone %Z %z", &targetTime);

       return String(buffer);
    }

    long timeUntil() {
        // Method to calculate the time until the target time

        // Calculate current time
        time_t currentTime = getCurrentTime();

        // Calculate target time
        time_t targetT = mktime(&targetTime);

        // Calculate difference
        return difftime(targetT, currentTime);
    }

    void incrementDays(int days) {
        // Method to increment the target time by a specified number of days

        // Get current target time
        time_t currentTime = mktime(&targetTime);

        // Add days to target time
        currentTime += (days * 3600 * 24);

        /* Update target time
        targetTime is initialized using an empty {} initializer to reset all its members.
        The localtime_r() function is then used to populate targetTime with the components of the new time after adding the specified number of days. 
        This approach ensures that targetTime is correctly updated with the new time information and resolves the error related to the assignment operator.
        */
        targetTime = {};
        localtime_r(&currentTime, &targetTime);
    }

    int getDay(){
        // Method to get the day of the target time
        
        return targetTime.tm_mday;
    }
};

#endif