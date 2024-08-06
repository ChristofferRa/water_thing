/*
  Sleep functions
    
  By Christoffer Rappmann, christoffer.rappmann@gmail.com

    The purpose of this file is to facilitate putting the ESP32 microcontroller into deep sleep mode, 
    conserving power during idle periods. It includes functions to initiate sleep, print wake-up 
    reasons, and manage wake-up sources. Additionally, it retains essential variables across deep 
    sleep cycles to maintain state information.

  sleepSetup():
      Purpose: Initializes settings and prints boot information.
      Parameters: None.
      Functionality: Increments the boot count, prints it, and prints the wake-up reason.

  sleepNow(int sToSleep):
    Purpose: Initiates the deep sleep mode for the specified duration.
    Parameters:
      sToSleep: The duration in seconds for the ESP32 to remain in deep sleep.
    Functionality: Enables the wake-up timer and external wake-up buttons, prints a message indicating the sleep duration, and initiates the deep sleep mode.
  
  print_wakeup_reason():
    Purpose: Prints the reason for the ESP32 waking up from sleep.
    Parameters: None.
    Functionality: Retrieves the wake-up cause and prints a message indicating the reason for the wake-up.

  Retained Variables (RTC_DATA_ATTR):
    bootCount: Stores the number of times the ESP32 has booted.
    lastWaterDay: Stores the day of the last watering action.
*/

// Sleep
// https://randomnerdtutorials.com/esp32-deep-sleep-arduino-ide-wake-up-sources/

#include <Arduino.h>
#include "sleep.h"

#define uS_TO_S_FACTOR 1000000ULL  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  60
//const long long timeToSleep = uS_TO_S_FACTOR * 10 * 1;

// wake-up buttons
#define BUTTON_PIN_BITMASK 0x8004 // GPIOs 2 and 15

//retain variables after sleep
RTC_DATA_ATTR int bootCount = 0;
RTC_DATA_ATTR int lastWaterDay = 40; // day of last watering, set to a date that does not exist


void sleepSetup(){

    //Increment boot number and print it every reboot

    ++bootCount;

    Serial.println("\nBoot number: " + String(bootCount));

    // Why did it wake?
    print_wakeup_reason();

}

void sleepNow(int sToSleep){
    // Start deep sleep
    
    // Define wake up timer (sleep time) and wakeup buttons 
    //esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
    esp_sleep_enable_timer_wakeup(sToSleep * uS_TO_S_FACTOR);
    esp_sleep_enable_ext1_wakeup(BUTTON_PIN_BITMASK,ESP_EXT1_WAKEUP_ANY_HIGH);

    Serial.flush();
    Serial.println("\nGoing to sleep for " + String(sToSleep ) + " s");
    esp_deep_sleep_start();
}


void print_wakeup_reason(){
    /*
    Method to print the reason by which ESP32
    has been awaken from sleep

    https://lastminuteengineers.com/esp32-deep-sleep-wakeup-sources/

    */
    esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 :        Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 :        Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER :       Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD :    Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP :         Serial.println("Wakeup caused by ULP program"); break;
    default :                           Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
  }
}
