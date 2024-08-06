#include "water_settings.h"
#include "config.h"

void settingsMQTT(String message){
    // This function will be called when a settingsMQTT has been recieved.
    // It should recieve a json file with settings...
    Serial.println("\nApplying new settings!");
    settings.extractSettingsJSON(message.c_str());
    settings.printExtractedIntegers();
}