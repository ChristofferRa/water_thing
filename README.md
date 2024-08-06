# water_thing
ESP32 Powered watering system  

## Basic Functionality
Open a motorized ball valve at a specific time each day for x minutes.  
Water pressure, valve state and battery level is reported via MQTT.

Time to wich to water, duration of watering, battery and pressure warning levels etc. may be updated from default values via MQTT.

## Hardware  
This is the code for my watering system consisting of:  
- 12 V Lead-Acid battery
- ESP32 WROOM 32U (With external antenna)
- 12 -> 5v step down regulator
- Automotive pressure sensor (0-5v) (0-30 psi)
- 2 x MOSFET trigger switch module
- 12 V motorized ball valve
- 3 x LED RED/GREEN/ORANGE
- Resistors, 100 kOhm / 30 kOhm / 68 kOhm / 120 kOhm / 3 x 220 Ohm / 2 x 10 kOhm
- 2 x switches
- 3D printed enclosure, connectors and adapters

A schematic is provided [here](https://github.com/ChristofferRa/water_thing/blob/main/water_thing_schematic.pdf)
