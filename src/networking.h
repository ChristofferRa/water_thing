#ifndef NETWORKING_H
#define NETWORKING_H

/*
This code manages Wi-Fi connectivity in an Arduino project. 
It includes event handling functions to respond to connection-related events, 
a function to connect to a Wi-Fi network, and a function to disconnect from the Wi-Fi network.

By Christoffer Rappmann, christoffer.rappmann@gmail.com
*/

#include "credentials.h"

void connect_wifi(wifiCredentials cred);
void wifi_disconnect();

#endif