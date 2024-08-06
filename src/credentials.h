
#ifndef CREDENTIALS_H
#define CREDENTIALS_H
/*
Credentials

By Christoffer Rappmann, christoffer.rappmann@gmail.com

Defines ojects containing credentials for wifi and mqtt

wifiCredentials Class:
    Purpose: 
        Stores Wi-Fi connection credentials, including activation status, device name, SSID, and password.
    Private Variables: 
        wifi_active, device_name, ssid, password.
    Public Methods:
        wifiCredentials(bool active, String name, String network, String pass): Constructor to initialize Wi-Fi credentials.
        Getter methods for retrieving Wi-Fi credentials: getWifiActive(), getDeviceName(), getSSID(), getPassword().

mqttCredentials Class:
    Purpose: 
        Stores MQTT connection credentials, including activation status, device name, server address, port, username, password, and arrays of topics for publishing and subscribing.
    Private Variables: 
        active, device_name, server, port, user, password, pub, sub, pubSize, subSize.
    Public Methods:
        mqttCredentials(bool active, String device_name, String server, int port, String user, String password, String* pub, int pubSize, String* sub, int subSize): Constructor to initialize MQTT credentials.
        Getter methods for retrieving MQTT credentials: getActive(), getDeviceName(), getServer(), getPort(), getUser(), getPassword(), getPub(int i), getPubSize(), getSub(int i), getSubSize().
        Setter methods for updating arrays of topics: setPub(String* pubArray, int size), setSub(String* subArray, int size).
*/

#include <Arduino.h>

//***************************
//***        WiFi         ***
//***************************

class wifiCredentials {
    /*
    Class for storing Wi-Fi credentials.
    Stores information such as Wi-Fi activation status, device name, SSID, and password.
    */

    private:
        bool wifi_active;
        String device_name;
        String ssid;
        String password;

    public:
        // Constructor
        wifiCredentials(bool active, String name, String network, String pass)
            : wifi_active(active), device_name(name), ssid(network), password(pass) {}

        // Getter functions
        bool getWifiActive() const {
            return wifi_active;
        }

        String getDeviceName() const {
            return device_name;
        }

        String getSSID() const {
            return ssid;
        }

        String getPassword() const {
            return password;
        }
};

//***************************
//***        MQTT         ***
//***************************

class mqttCredentials {
    /*
    Class for storing MQTT credentials.
    Stores information such as MQTT activation status, device name, server address, port, username, password,
    as well as arrays of topics for publishing and subscribing.
    */

private:
    bool active;
    String device_name;
    String server;
    int port;
    String user;
    String password;

    String* pub; // Array of strings for publishing topics
    String* sub; // Array of strings for subscribing topics
    int pubSize; // Size of the pub array
    int subSize; // Size of the sub array

public:
    // Constructor
    mqttCredentials(bool active, String device_name, String server, int port, String user, String password, String* pub, int pubSize, String* sub, int subSize)
        : active(active), device_name(device_name), server(server), port(port), user(user), password(password), pub(pub), pubSize(pubSize), sub(sub), subSize(subSize) {}

    // Getter methods
    bool getActive() const {
            return active;
        }
    String getDeviceName() const {
        return device_name;
    }

    String getServer() const {
        return server;
    }

    int getPort() const {
        return port;
    }

    String getUser() const {
        return user;
    }

    String getPassword() const {
        return password;
    }

    //String* getPub() const {
    String getPub(int i) const {
        return pub[i];
    }

    int getPubSize() const {
        return pubSize;
    }

    String getSub(int i) const {
        return sub[i];
    }

    int getSubSize() const {
        return subSize;
    }

    // Setter methods for pub and sub arrays
    void setPub(String* pubArray, int size) {
        pub = pubArray;
        pubSize = size;
    }

    void setSub(String* subArray, int size) {
        sub = subArray;
        subSize = size;
    }
};


#endif