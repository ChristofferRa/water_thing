/*
This code manages Wi-Fi connectivity in an Arduino project. 
It includes event handling functions to respond to connection-related events, 
a function to connect to a Wi-Fi network, and a function to disconnect from the Wi-Fi network.

By Christoffer Rappmann, christoffer.rappmann@gmail.com

This setup ensures that the device can connect to a specified Wi-Fi network using the provided 
credentials and handles events such as successful connection, obtaining an IP address, and 
disconnection gracefully.

WiFi Event Handling Functions:
  WiFiStationConnected: Handles the event when the device successfully connects to the Wi-Fi access point (AP).
  WiFiGotIP: Handles the event when the device obtains an IP address after connecting to the Wi-Fi network.
  WiFiStationDisconnected: Handles the event when the device disconnects from the Wi-Fi access point and attempts to reconnect.

Function to Connect to WiFi (connect_wifi):
  This function attempts to connect to a Wi-Fi network using the provided credentials.
  Parameters: Takes an instance of the wifiCredentials class (cred) containing Wi-Fi network credentials.

Function to Disconnect WiFi (wifi_disconnect):
  This function disconnects the device from the current Wi-Fi network.
*/

#include "networking.h"

#include <WiFi.h>
#include "config.h"

// Event Handling
void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info){
  Serial.println("Connected to AP successfully!");
}

void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info){
  Serial.println("WiFi connected");
  Serial.println("\n\nConnected to; " + WiFi.SSID());
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info){
  Serial.println("Disconnected from WiFi access point");
  Serial.print("WiFi lost connection. Reason: ");
  Serial.println(info.wifi_sta_disconnected.reason);
  Serial.println("Trying to Reconnect");
  WiFi.begin(wifi_cred.getSSID(), wifi_cred.getPassword());
}

void connect_wifi(wifiCredentials cred){
  // Connect to wifi
  Serial.print("\nTrying to connect to WiFi: ");
  Serial.print(cred.getSSID());
  Serial.println("");
  
  // Make sure wifi is disconnected before trying to connect.
  WiFi.disconnect(true);

  // Listen for events
  WiFi.onEvent(WiFiStationConnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_CONNECTED);
  WiFi.onEvent(WiFiGotIP, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_GOT_IP);
  WiFi.onEvent(WiFiStationDisconnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
  
  // Wifi-setup
  WiFi.mode(WIFI_STA);
  WiFi.setTxPower(WIFI_POWER_8_5dBm); // Workaround for getting wifi working on ESP32-C3
  WiFi.hostname(cred.getDeviceName());

  delay(500);
  // Connect
  WiFi.begin(cred.getSSID(), cred.getPassword());
  
  int timeout_wifi = 0;
  while (WiFi.status() != WL_CONNECTED && timeout_wifi < 30) {
    // Wait for connection and show animation
    Serial.print(".");
    timeout_wifi++;
  }
  if (WiFi.status() == WL_CONNECTED){
    // If succesfully connected
    Serial.println("WiFi connected");
    Serial.println("\n\nConnected to: " + String(cred.getSSID()));
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    delay(1000);
  }else{
    // If not connected after time-out
    Serial.println("\n\nFailed to Connect to " + String(cred.getSSID()));
    delay(5000);
  }
}

void wifi_disconnect(){
    //Disconnect wifi
    Serial.print("\nDisconnecting from WiFi: ");
    Serial.print(WiFi.SSID());
    Serial.println("");
    WiFi.disconnect(true);

}