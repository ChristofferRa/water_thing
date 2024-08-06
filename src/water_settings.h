#ifndef SETTINGS_H
#define SETTINGS_H

#include <Arduino.h>

/*
This file is supposed to handle settings for waterThing.
i.e. define a functions for changing settings and saving to file system

By Christoffer Rappmann, christoffer.rappmann@gmail.com

settings is recieved as json from MQTT.
Communication with the MQTT to a node red script according to: (to be implemented)
1. When ready to recieve new settings a ready message is sent on topic xxxx
2. Node red script will send a message coded in json with all settings
3. An instance of class waterSettings containing all active settings can be suplied with json-formated string which will den extract and update all settings
*/

#include <ArduinoJson.h>

struct timeHHMM {
    // Struct containing a time of day
    // HH:MM

    int HH;
    int MM;
};


class waterSettings {
    /* Class that extracts 
    timeToWater      = how many minutes is each watering session?
    waterTime        = at which time of day should watering begin?
    batteryLow       = at which battery voltage should a low battery warning be sent out?
    levelLow         = at which water level in tank should a low water level warning be sent out?
    defaultSleepTime = how many minutes to sleep between each wake upp
    waterOnDemand    = do an extra watering on demand, ie when recivied. 
    skipWatering     = no watering today thanks...

    from a Json-string
    */

private:
    int timeToWater;
    timeHHMM waterTime;
    int batteryLow;
    int levelLow;
    int defaultSleepTime;
    bool waterOnDemand;
    bool skipWatering;

public:
    // Constructor
    waterSettings(timeHHMM wTime, int tTime, int btrLow, int lvlLow, int defSleepTime) 
            : waterTime(wTime), timeToWater(tTime), batteryLow(btrLow), levelLow(lvlLow), defaultSleepTime(defSleepTime) {
                waterOnDemand = false;
                skipWatering = false;
            }

    // Getter function for timeToWater
    int getTimeToWater() const {
        return timeToWater*60;
    }

    // Getter function for waterTime
    int getWaterTimeHour() const {
        return waterTime.HH;
    }

    // Getter function for waterTime
    int getWaterTimeMinute() const {
        return waterTime.MM;
    }

    // Getter function for batteryLow
    int getBatteryLow() const {
        return batteryLow;
    }

    // Getter function for levelLow
    int getLevelLow() const {
        return levelLow;
    }

    // Getter function for default sleep
    int getDefaultSleepTime() const {
        return defaultSleepTime;
    }

    // Method to extract settings from JSON formatted data
    void extractSettingsJSON(const char* jsonData) {
        StaticJsonDocument<200> doc;
        DeserializationError error = deserializeJson(doc, jsonData);

        if (error) {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
            return;
        }

        if (doc.containsKey("timeToWater")) {
            timeToWater = doc["timeToWater"];
        }
        if (doc.containsKey("waterTime")) {
            JsonObject waterTimeObject = doc["waterTime"];
            waterTime.HH = waterTimeObject["HH"];
            waterTime.MM = waterTimeObject["MM"];
        }
        if (doc.containsKey("batteryLow")) {
            batteryLow = doc["batteryLow"];
        }
        if (doc.containsKey("levelLow")) {
            levelLow = doc["levelLow"];
        }
        if (doc.containsKey("defaultSleepTime")) {
            defaultSleepTime = doc["defaultSleepTime"];
        }
        if (doc.containsKey("waterOnDemand")) {
            waterOnDemand = doc["waterOnDemand"];
        }
        if (doc.containsKey("skipWatering")) {
            skipWatering = doc["skipWatering"];
        }
    }

    void printExtractedIntegers() {
        
        Serial.print("\ntimeToWater: ");
        Serial.println(timeToWater);
        Serial.print("waterTime - HH: ");
        Serial.println(waterTime.HH);
        Serial.print("waterTime - MM: ");
        Serial.println(waterTime.MM);
        Serial.print("batteryLow: ");
        Serial.println(batteryLow);
        Serial.print("levelLow: ");
        Serial.println(levelLow);
        Serial.print("defaultSleepTime: ");
        Serial.println(defaultSleepTime);
    }
};

// Functions from CPP-file that should be accessible
void settingsMQTT(String);

#endif