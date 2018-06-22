# BatteryCharger
Code for Battery Charger (implemented and external arduino)
This code is built for following circuitboard:
https://workspace.circuitmaker.com/Projects/Details/Jeroen-Schelkens/Battery-Charger-SMD

This code repository is used to communicate with and drive a self made battery charger. All the communication is done over the I2C protocol.

The "Slave" code needs to be uploaded to a arduino chip and has a PI loop for current and voltage limitting to the battery and load.

The "Master" code can send the desired current and voltage values, and receives the measured voltage and current and can be implemented in other pieces of code.
