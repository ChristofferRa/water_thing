#ifndef CONFIG_H
#define CONFIG_H

/*
Configuration file (se config.cpp) for easy management of settings for water_thing project.
settings object are made global in this file.

By Christoffer Rappmann, christoffer.rappmann@gmail.com
*/

#include "water_settings.h"
#include "credentials.h"

// Define global variable with credentials to be used

// Basic Settings
extern waterSettings settings;

// WiFi
extern wifiCredentials wifi_cred;

// MQTT
extern mqttCredentials mqtt_cred; 
#endif