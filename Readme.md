# Macchina CAN to UART

Parses data from the Macchina M2 CAN transceivers and forwards along over UART.
This project is used as the first stage of using car data to control an Xbox controller

* The project is currently setup to work with a Subaru BRZ. Future improvements: load a DBC file with signal definitions.

## Supported Signals:

* Accelerator Pedal Position
* Brake Pedal Position
* Clutch Engaged
* Steering Wheel Angle
* E-brake Engaged
* 