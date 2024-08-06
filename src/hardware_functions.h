#ifndef HARDWARE_FUNCTIONS_H
#define HARDWARE_FUNCTIONS_H

/*
hardware_functions
By Christoffer Rappmann, christoffer.rappmann@gmail.com 

This file contains classes for manageing the specific hardware feauters of the waterThing system

sensors Class:
    Class for reading and storing sensor values.
    Provides methods to read pressure and battery level,
    and stores the sensor data along with warning flags for low levels.

    Private Variables:
        prSensorPin: Pin for the pressure sensor.
        btrLvlPin: Pin for the battery level sensor.
        Sensor data variables: pressure, tankLevel, batteryVoltage.
        Warning flags: warningLowLevel, warningLowBattery.

    Public Functions:
        sensors(int levelLow, int batteryLow): Constructor to initialize the sensor class with low-level warning and low-battery warning thresholds.
        readSensors(): Method to update sensor values.
        Getter functions for sensor data and warning flags: getPressure(), getLevel(), getBatteryVoltage(), getWarningLowLevel(), getWarningLowBattery().

valve Class:
    Class for managing the water valve.
    Provides methods to open, close the valve and reporting valve state.

    Private Variables:
        vlvOpenPin: Pin for opening the valve.
        vlvClosePin: Pin for closing the valve.

    Public Functions:
        valve(): Constructor to initialize the valve class.
        open(): Method to open the valve.
        close(): Method to close the valve.

leds Class:
    Class for controlling LEDs.
    Provides methods to turn individual LEDs on and off.

    Private Variables:
        Pins for LEDs: ledD1, ledD2, ledD3.

    Public Functions:
        leds(): Constructor to initialize the LED class.
        Functions to control each LED individually: redLedOn(), redLedOff(), orangeLedOn(), orangeLedOff(), greenLedOn(), greenLedOff().

buttons Class:
    Class for reading the state of switches.
    Provides methods to check if each switch is pressed or not.

    Private Variables:
        Pins for switches: buttonSW1, buttonSW2.

    Public Functions:
        Switch(): Constructor to initialize the switch class and set pin modes.
        Method to check if switch SW1 is pressed: isPressedSW1().
        Method to check if switch SW2 is pressed: isPressedSW2().
        Method to check if any switch is pressed (returns true if any button is pressed): isAnyPressed().
*/

#include <Arduino.h>
#include <Wire.h>

// Global variable for valve state, it is retained after sleep.
extern RTC_DATA_ATTR bool valveState;

class sensors {
    /*
        Class for reading and storing sensor values.
        Provides methods to read pressure and battery level,
        and stores the sensor data along with warning flags for low levels.
    */
    private:
        // Pins used by sensors
        static const int prSensorPin = 33; // Pressure sensor pin
        static const int btrLvlPin = 35; // Battery level pin

        // Sensor data
        double pressure;        // Actual water pressure, barg
        double tankLevel;       // Corresponding tank level, %
        double batteryVoltage;  // Battery voltage, V

        // Warnings
        double levelLow;
        double batteryLow;

        bool warningLowLevel;
        bool warningLowBattery;

        static double adc2voltage(double adc_val){
            /* Function for compensating ADC value for unlinearity
            ADC correction
            [ 1.75101646e-01  7.25018385e-04  8.88075249e-08 -2.20849715e-11]
            https://randomnerdtutorials.com/esp32-adc-analog-read-arduino-ide/

            adc_val: double, value between 0-4095 from ADC

            returns, double corrected voltage that corresponds to reading
            */
            double x0 = 0.175101646;
            double x1 = 0.000725018385;
            double x2 = 0.0000000888075249;
            double x3 = 0.0000000000220849715;

            double U = x0 + pow(x1, 1)*adc_val + pow(x2, 2)*adc_val + pow(x3, 3)*adc_val;
            return U;
        }
    
        void readPressure(){
            /* Function for reading a voltage from specified pins and converting it into a pressure
            */
            double R6 = 67.3 * 1000; // Resistance R6 ohm
            double R7 = 117.3 * 1000; // Resistance R7 Ohm
  
            //double adc_val = analogRead(prSensorPin); //Read adc value
            // Sample adc 5 times and average
            double adc_val = 0;
            int nrSamples = 5;
            for (int i=0; i<nrSamples; i++){
                adc_val = adc_val + analogRead(prSensorPin); //Read adc value
            delay(200);
            }
            adc_val = adc_val / nrSamples;
  
            double U = adc2voltage(adc_val);
            //Serial.print("\nU: " + String(U, 4) + "V (" + String(adc_val, 4) + "/4095)");
  
            double I = U / R7 * 1e6; // Calculate current
            double Uactual = (R6 + R7) / R7 * U; // Voltage from pressure sensor
            //Serial.print("\nUactual: " + String(Uactual, 4));
  
            // Pressure sensor outputs 0.5V (0 PSig) - 4.5V (30 PSig)
            //                              (0 bar(e))       (2.068 bar(e))
            // Output voltage is linear.
            // Suply voltage to sensor = 5V

            double uHigh = 4.5;
            double uLow = 0.5;
            double pHigh = 2.068;
            double pLow = 0.0;

            double k = (pHigh - pLow) / (uHigh - uLow);
            //Serial.print("\nk: " + String(k, 4));
            double m = pLow - uLow * k;
            //Serial.print("\nm: " + String(m, 4));
            pressure = k * Uactual + m;
            }

        void readBatteryLevel() {
            /* Function for reading battery level/voltage*/
            double R1 = 100.0 * 1000; // Resistance R1 ohm
            double R2 = 30.0 * 1000; // Resistance R2 Ohm

            // Sample adc 5 times and average
            double adc_val = 0;
            int nrSamples = 5;
            for (int i=0; i<nrSamples; i++){
                adc_val = adc_val + analogRead(btrLvlPin); //Read adc value
                delay(200);
            }
            adc_val = adc_val / nrSamples;
  
            double U = adc2voltage(adc_val); //seems to show 2v to little...
            //Serial.print("\nU: " + String(U, 4) + "V (" + String(adc_val, 4) + "/4095)");
  
            double I = U / R2 * 1e6; // Calculate current
            batteryVoltage = (R1 + R2) / R2 * U; // Voltage before voltage divider
  
            }

    public:
        // Constructor
        sensors(double levelLow, double batteryLow)
            : levelLow(levelLow), batteryLow(batteryLow){
                warningLowLevel = false;
                warningLowBattery = false;
            }

        void readSensors(){
            /* Update all available sensors and store in object*/
            // Read pressure
            Serial.println("Reading pressure....");
            readPressure();
            Serial.println("Pressure " + String(pressure) + " bar(e)");

            //Calculate Level
            tankLevel = pressure/998.0/9.82*1e5;

            // Warn if pressure is below levelLow
            if (tankLevel < levelLow){
                warningLowLevel = true;
            }

            // Read battery level
            Serial.println("Reading battery level....");
            readBatteryLevel();
            Serial.println("Level " + String(batteryVoltage) + " V");

            // Warn if batteryvoltage is below 11 V
            if (batteryVoltage < batteryLow){
                warningLowBattery = true;
            }
        }

        void updateWarningLevels(double lvl, double btr){
            levelLow = lvl;
            batteryLow = btr;
        }

        // Getter function for pressure
        double getPressure() const {
        return pressure;
        }

        // Getter function for tankLevel
        double getLevel() const {
        return tankLevel;
        }

        // Getter function for batteryVoltage
        double getBatteryVoltage() const {
        return batteryVoltage;
        }

        bool getWarningLowLevel() const{
            return warningLowLevel;
        }

        bool getWarningLowBattery() const{
            return warningLowBattery;
        }


};

class valve{
    /*
        Class for managing the water valve.
        Provides methods to open, close the valve and reporting valve state.
    */
    private:
        // Pins used by valve
        static const int vlvOpenPin = 25;
        static const int vlvClosePin = 26;

    public:
        // Constructor
        valve(){
            // Set used pins to output...
            pinMode(vlvOpenPin, OUTPUT);
            pinMode(vlvClosePin, OUTPUT);
        }

        // Open valve.
        void open() {
  
            Serial.print("\n\n -- Opening valve --\n");
            //digitalWrite(ledD2, HIGH);
      
            digitalWrite(vlvOpenPin, HIGH);
            delay(10 * 1000); // valve takes roughly 8 s to manouver
            digitalWrite(vlvOpenPin, LOW);
            valveState = true; // Set global variable valveState, its global to be able to be saved during sleep
        }

        // Close valve.
        void close() {
  
            Serial.print("\n\n -- Closing valve --\n");
            //digitalWrite(ledD2, LOW);
      
            digitalWrite(vlvClosePin, HIGH);
            delay(10 * 1000); // valve takes roughly 8 s to manouver
            digitalWrite(vlvClosePin, LOW);
            valveState = false; // Set global variable valveState, its global to be able to be saved during sleep
        }
};

class leds{
    /*
        Class for controlling LEDs.
        Provides methods to turn individual LEDs on and off.
    */
    private:
        // Pins used for leds
        static const int ledD1 = 18; //RED
        static const int ledD2 = 19; //Orange
        static const int ledD3 = 17; //Green

    public:
    // Constructor
    leds(){
        // Set used pins to output...
        pinMode(ledD1, OUTPUT);
        pinMode(ledD2, OUTPUT);
        pinMode(ledD3, OUTPUT);
    }

    // Function to turn on the red LED
    void redLedOn() {
        digitalWrite(ledD1, HIGH);
    }

    // Function to turn off the red LED
    void redLedOff() {
        digitalWrite(ledD1, LOW);
    }

    // Function to turn on the orange LED
    void orangeLedOn() {
        digitalWrite(ledD2, HIGH);
    }

    // Function to turn off the orange LED
    void orangeLedOff() {
        digitalWrite(ledD2, LOW);
    }

    // Function to turn on the green LED
    void greenLedOn() {
        digitalWrite(ledD3, HIGH);
    }

    // Function to turn off the green LED
    void greenLedOff() {
        digitalWrite(ledD3, LOW);
    }

};

class buttons {
    /*
    Class for reading switch positions.
    Provides methods to check if switches SW1 and SW2 are pressed.
    */
private:
    // Pins used for switches
    static const int buttonSW1 = 15;
    static const int buttonSW2 = 2;

public:
    buttons() {
        pinMode(buttonSW1, INPUT_PULLDOWN);
        pinMode(buttonSW2, INPUT_PULLDOWN);
    }

    // Method to check if switch SW1 is pressed
    bool isPressedSW1() {
        return digitalRead(buttonSW1);
    }

    // Method to check if switch SW2 is pressed
    bool isPressedSW2() {
        return digitalRead(buttonSW2);
    }

    // Method to check if any switch is pressed
    bool isAnyPressed() {
        return (digitalRead(buttonSW1) || digitalRead(buttonSW2));
    }
};

#endif