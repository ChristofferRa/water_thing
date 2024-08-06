#include <Arduino.h>

#include "networking.h"
#include "mqtt_handler.h"
#include "config.h"
#include "water_settings.h"
#include "multitasker.h"
#include "sleep.h"
#include "time_keeping.h"
#include "hardware_functions.h"

mqttHandler* mqttSession = nullptr; // Declare pointer to mqttHandler
  
// Set up target time
timeKeeper* targetTime = nullptr;

// Set upp valve, sensors and leds, do outside of setup to only do once
valve myValve;
sensors mySensors(settings.getLevelLow(), settings.getBatteryLow());
leds myLeds;
buttons mybuttons;

void setup() {
  //***************
  //**** Setup! ***
  //***************

  // Start Serial
  Serial.begin(115200);

  // Setup deep sleep
  sleepSetup();

  // If active, connect to wifi
  if (wifi_cred.getWifiActive()) {
    connect_wifi(wifi_cred);
    if (WiFi.status() == WL_CONNECTED){
      myLeds.greenLedOn();
      }
    else{
      myLeds.orangeLedOn();
      }
    }
  else {
    Serial.println("Wifi not active");
    myLeds.greenLedOn();
    }

  // If MQTT shoud be active, instantiate mqttSession
  if (mqtt_cred.getActive()){
    mqttSession = new mqttHandler(mqtt_cred); // Dynamically allocate memory and instantiate mqttHandler
    
    //Test subscription
    //Serial.println("Test to subsript to " + String(mqtt_cred.getSub(0)));
    //mqttSubs = new mqqtSubscriptions();
    //mqttSubs.addSub(mqtt_cred.getSub(0), settingsMQTT);
    //mqttSession->addSubscription(mqtt_cred.getSub(0), settingsMQTT);
  } 
  else {
    Serial.println("MQTT not active");
  }

  // Connect to NTP server and set up target time
  timeSetup();
  if(bootCount < 2){ // If first boot wait for time to sync
    delay(10000); // Make sure timeserver is connected
  }

  // Close valve if not closed
  if (valveState){
    
    // Send Valve state
    mqttSession->publish(mqtt_cred.getPub(0), String(valveState));

    // Close Valve
    myValve.close();
  }

  //***********************
  //**** Do your thing! ***
  //***********************

  //----------------------
  // Try updating settings
  //----------------------
  Serial.println("\n\n1. Updating settings");
  
  // Setup
  mqttSession->addSubscription(mqtt_cred.getSub(0), &settingsMQTT);
  mqttSession->publish(mqtt_cred.getPub(3), "Ready");
    
  //Listen for a respons for 1s
  unsigned long startTime = millis();
  while (millis() - startTime < 1000) {
    mqttSession->loop();
    delay(100);
  }

  // -------------
  // Check sensors
  // -------------
  Serial.println("\n\n2. Checking my sensors....");
  mySensors.updateWarningLevels(settings.getBatteryLow(), settings.getLevelLow());
  mySensors.readSensors();
  
  // Turn on warning lights correspondingly
  if(mySensors.getWarningLowBattery()){
    Serial.println("Warning, low battery level!");
    myLeds.redLedOn();
  }

  if(mySensors.getWarningLowLevel()){
    Serial.println("Warning, low water level!");
    myLeds.redLedOn();
  }

   // ------------------
  // Send data via MQTT
  // ------------------

  if (mqtt_cred.getActive()){ //only if MQTT Active
    Serial.println("\n\n3. Send MQTT Data");

    // Valve state
    mqttSession->publish(mqtt_cred.getPub(0), String(valveState));
    // Level
    delay(100); // Add a small delay to make sure all messages are sent.
    mqttSession->publish(mqtt_cred.getPub(1), String(mySensors.getLevel(), 2));

    // Pressure
    delay(100);
    mqttSession->publish(mqtt_cred.getPub(4), String(mySensors.getPressure(), 2));

    // Battery
    delay(100);
    mqttSession->publish(mqtt_cred.getPub(2), String(mySensors.getBatteryVoltage(),2));
  }

  // ---------------------
  // If time to water, do!
  // ---------------------

  Serial.println("\n\n4. Is it time? To water?");
  targetTime = new timeKeeper(settings.getWaterTimeHour(), settings.getWaterTimeMinute());
  
  Serial.println("Current target time:" + targetTime->timeString());
  Serial.println("Time until target: " + String(targetTime->timeUntil()) + " s");

  if (targetTime->timeUntil() <= 0){
      Serial.println("Time has passed");
      if (targetTime->getDay() != lastWaterDay){ // check which day the last watering occured, if not today, then water..
        Serial.println("Not watered yet today, do the watering...");

        myValve.open(); // Open valve
        lastWaterDay = targetTime->getDay(); // Set last water dat to today
        Serial.println("\nValve State:" + String(valveState));
        
        // Send new Valve state
        mqttSession->publish(mqtt_cred.getPub(0), String(valveState));
        delay(100); // wait for 100ms to make sure message is sent before going to sleep.

        sleepNow(settings.getTimeToWater()); // Sleep for the duration of the watering
      }
      else{
        Serial.println("Already watered today, do nothing...");
      }
    }

    // Manual override to water, If a button is pressed the valve should be opened
    if (mybuttons.isAnyPressed()){
      // Is any button pressed?
      Serial.println("Manual override, opening valve....");
      myValve.open(); // Open valve

      // Send new Valve state
      mqttSession->publish(mqtt_cred.getPub(0), String(valveState));

      sleepNow(settings.getTimeToWater()); // Sleep for the duration of the watering

      /*
        The valve will remain open for the duration of a standard watering. 
        
        If any button is pressed again the device will wake from sleep and close the valve. 
        The valve is closed on every boot if open.
      */
    }

  // -----------
  // Go to sleep
  // -----------

  Serial.println("\n\n5. Preparing to sleep...");

  // if time to next watering is less than the default sleep time and no watering has been done yet today
  // the sleep time should be shortened to next watering time.
  if ((settings.getDefaultSleepTime() > targetTime->timeUntil()) && (targetTime->getDay() != lastWaterDay)){
    sleepNow(targetTime->timeUntil());
  } else { // if not, use default sleep time
    sleepNow(settings.getDefaultSleepTime());
  }

}

void loop() {
  // put your main code here, to run repeatedly:
  
  // this should never be reached.
}