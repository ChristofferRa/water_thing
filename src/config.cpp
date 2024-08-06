
/*
Configuration file (se config.cpp) for easy management of settings for water_thing project.

By Christoffer Rappmann, christoffer.rappmann@gmail.com
*/


#include "config.h"
#include "credentials.h"
#include "water_settings.h"

//******************
// Basic Settings
//******************
#define waterTimeHH 20   // Hour to commence watering
#define waterTimeMM 00   // Minute during above our to commence watering

#define timeToWater 5   // min, how many minutes to water
#define batteryLow  11.0   // V, Low voltage level alarm for battery
#define levelLow    5.0 // m, Low level alarm for water tank

#define defSleepTime 60  // S, default sleep time

timeHHMM waterTime = {waterTimeHH, waterTimeMM};

waterSettings settings(waterTime, timeToWater, batteryLow, levelLow, defSleepTime);

//******************
// Wifi credentials
//******************

#define wifiactive true
#define deviceName "water_thing"
#define ssid "YOUR_SSID"
#define pwd "YOUR_PASSWORD"

// Init wifi object
wifiCredentials wifi_cred(wifiactive, deviceName, ssid, pwd);

//******************
// MQTT credentials
//******************

// Server and credentials
#define mqtt_active true

#define mqtt_server "YOUR_MQTT_SERVER_IP"
#define mqtt_port 1883
#define mqtt_user "YOUR_MQTT_USER"
#define mqtt_password "YOUR_MQTT_PASSWORD"

// pub topics
#define water_vlv_state "sensors/water_thing/vlv_state"
#define water_level "sensors/water_thing/level"
#define water_voltage "sensors/water_thing/battery_voltage"
#define water_pressure "sensors/water_thing/pressure"

#define water_ready "water_thing/ready" // Topic to send ready message to for updating settings

//sub topics
#define update_settings_mqtt "water_thing/settings" // send settings to  waterThing on this topic

// Init mqtt object
String pubs[] = {water_vlv_state, water_level, water_voltage, water_ready, water_pressure};
String subs[] = {update_settings_mqtt};

// mqttCredentials(bool active, String device_name, String server, String port, String user, String password, String* pub, int pubSize, String* sub, int subSize)
// By passing &pubs[0] and &subs[0], you are passing pointers to the first elements of the arrays pubs and subs, respectively, which is what the constructor expects
mqttCredentials mqtt_cred(mqtt_active, deviceName, mqtt_server, mqtt_port, mqtt_user, mqtt_password, &pubs[0], sizeof(pubs) / sizeof(pubs[0]), &subs[0],  sizeof(subs) / sizeof(subs[0]));