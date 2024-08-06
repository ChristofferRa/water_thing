#ifndef MQTTT_HANDLER_H
#define MQTTT_HANDLER_H

/*
MQTT handeler 
By Christoffer Rappmann, christoffer.rappmann@gmail.com

Contains main class mqttHandler and supporting functions/classes.
mqttHandler Class for connecting to a MQTT-server and handling
subscribed topics as well as publishing to a topic 
on the specific server.

mqttHandler: 
    This class handles the connection to the MQTT server and the publishing/subscribing of messages. 
    It uses the PubSubClient library for MQTT communication. 
    Key functions include:
        Functions: 
        - mqttInit() for initializing the MQTT client,
        - reconnect() for reconnecting to the MQTT server if the connection is lost 
        - loop() for checking the MQTT connection and handling messages in the main loop, and publish() for publishing messages 
            to a topic.
        - public(topic, message) Publish mqtt "pubMessage" on topic "pubTopic"
        
        Static functions:
        - callback() as the callback function for received MQTT messages, This is a static member function of mqttHandler, 
            acting as the callback for received MQTT messages. It matches the received topic with subscribed topics and calls 
            the corresponding functions.
        -addSubscription(), This is a static member function of mqttHandler, providing a way to add subscriptions to the global 
            mqttSubs instance of mqqtSubscriptions.


mqqtSubscriptions: 
    This class manages a list of subscriptions 
    (topics and corresponding functions to call when a message is received on that topic). 
    It provides methods for adding subscriptions (addSub()), getting the number of subscriptions 
    (getSubscriptionCount()), and retrieving a specific subscription (getSubscription()).

FunctionTopicPair: 
    This struct represents a pair of a topic and a function pointer. 
    It is used within mqqtSubscriptions to associate topics with functions.
*/

#include <Arduino.h>
#include "credentials.h"

// https://github.com/knolleary/pubsubclient
#include <WiFi.h>
#include <PubSubClient.h>

// Define the typedef for a function pointer
typedef void (*FunctionPointer)(String);

struct FunctionTopicPair {
    // Simple structure containing a topic with a corresponding function to call if a message is recieved on that topic
    // Used in mqqtSubscriptions class
    String topic;
    FunctionPointer functPtr;
};

class mqqtSubscriptions {
    /*
    This class contains a stack with structs of type "FunctionTopicPair", each such pair contains both
    a topic and a function pointer. 

    public functions for adding a new function-topic-pair to the stack, 
    a function for returning the number of elements in the stack 
    as well as function that returns the function-topic-pair for a given location in the stack 

    (the function that is pointed to is to be called when a MQTT message on that topic is recived., this
     is done by the mqttHandler class)
    */

    private:
        // Subscriptions
        FunctionTopicPair* subscriptions; // Struct with functionpointer and topic for subscribed topics.
        int subscriptionCount;            // Keep track of the number of subscriptions

    public:
    
        //Constructor
        mqqtSubscriptions():subscriptions(nullptr), subscriptionCount(0)
        {}

        void addSub(const String& topic, FunctionPointer functPtr) {
        // Method to add a new subscription to the array

        // Increase the size of the array
        FunctionTopicPair* newSubscriptions = new FunctionTopicPair[subscriptionCount + 1];
        // Copy existing subscriptions to the new array
        for (int i = 0; i < subscriptionCount; ++i) {
            newSubscriptions[i] = subscriptions[i];
        }
        // Add new subscription
        newSubscriptions[subscriptionCount].topic = topic;
        newSubscriptions[subscriptionCount].functPtr = functPtr;
        // Delete the old array
        delete[] subscriptions;
        // Update the pointer to the new array
        subscriptions = newSubscriptions;
        // Increment subscription count
        subscriptionCount++;
        }

        int getSubscriptionCount() {
            return subscriptionCount;
        }

        FunctionTopicPair getSubscription(int i){
            return subscriptions[i];
        }

};

// Global instance to share subscriptions
// This is done since it's not possible to pass an instance of an object to
// the callbackfunction within the mqttHandler class. 
extern mqqtSubscriptions mqttSubs;

class mqttHandler {
    /* Class for connecting to a MQTT-server and handling
    subscribed topics as well as publishing to a topic 
    on the specific server.
    */

    private:
        // Credentials and topics names
        mqttCredentials cred;

        // Clients for MQTT connection
        WiFiClient espClient;
        PubSubClient client;

        void mqttInit(){
            /*****************
            --- MQTT Init ---
            ******************/
            IPAddress ipAddress;

            ipAddress.fromString(cred.getServer());
    
            client.setServer(ipAddress, cred.getPort());
            client.setCallback(callback);

        }

        static void callback(char* topic, byte* message, unsigned int length) {
            /* Function that is passed to client as a callback for when a
            mqtt message has been recieved.

            It prints the topic and message to serial, converts the messsage to a string
            and then matches it to the correct subscribed topic.

            With each topic a function pointer to a global function is bundeled in a struct
            called "FunctionTopicPair". This function will then be called if the topic matches.
            */

            String messageTopic = String(topic);
            String messageTemp;
            
            Serial.println("Message arrived on topic: " + messageTopic + ". Message: ");

            // Read message
            for (int i = 0; i < length; i++) {
                //Serial.print((char)message[i]);
                messageTemp += (char)message[i];
            }
            Serial.println(messageTemp);

            // Match topic and call corresponding function
            for (int j = 0; j < mqttSubs.getSubscriptionCount(); j++){
                FunctionTopicPair tempPair = mqttSubs.getSubscription(j);
                if (messageTopic == tempPair.topic){
                    (*tempPair.functPtr)(messageTemp);
                }
            }
        }

        void reconnect() {
            // Function to connect to MQTT-server

            // Loop until we're reconnected
            while (!client.connected()) {
                Serial.println("Attempting MQTT connection...");

                // Create client ID
                String clientId = cred.getDeviceName();

                // Attempt to connect
                if (client.connect(clientId.c_str(), cred.getUser().c_str(), cred.getPassword().c_str())) {
                    Serial.println("MTTQ connected");
                    Serial.print("Server: ");
                    Serial.println(cred.getServer());

                //subscribe
                for (int i = 0; i< cred.getSubSize(); i++){
                    Serial.println("Subscribing to: " + cred.getSub(i));
                    client.subscribe(cred.getSub(i).c_str());
                }

                } else {
                    Serial.print("failed, rc=");
                    Serial.print(client.state());
                    Serial.println(" try again in 5 seconds");

                    // Wait 5 seconds before retrying
                    delay(5000);
                }
            }         
        }

    public:
        //Constructor
        mqttHandler(mqttCredentials cred)
        :cred(cred), client(espClient)
        {
            mqttInit(); 
            }

        void loop(){
            //Check MQTT-server connection and listen for MQTT-messages
            //if (cred.getActive()){
                if (!client.connected()) {
                    reconnect();
                }
                client.loop();
            //}
        }

        void publish(String pubTopic, String pubMessage){
            // Publish mqtt "pubMessage" on topic "pubTopic"
            if (!client.connected()) {
                    reconnect();
                }
                
            client.publish(pubTopic.c_str(), pubMessage.c_str());
        }

        static void addSubscription(const String& topic, FunctionPointer functPtr){
            // Wrapper to be used with MQTTHandler
            // Function to add a subscription to the glocal mqttSubs instance of
            // mqttSubscriptions. 
            
            mqttSubs.addSub(topic, functPtr);
        }
};

#endif