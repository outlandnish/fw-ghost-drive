# Ghost Drive - the Sim Racing Controller for Your Car

Ditch the expensive sim racing rigs and use your car instead to control racing games on either your PC or Xbox. This firmware is designed for the Macchina M2. It reads CAN bus data from your car and acts as an HID Gamepad. When paired with an Xbox Adaptive Controller, you can use it with Xbox Ones as well.

* Full write up of the project here: https://outlandnish.com/hacks/ditch-the-sim-rig-use-your-car-instead/
* Video demo of it in action: https://www.youtube.com/watch?v=qct7rrOwh6s

The project is setup to work with a Subaru BRZ. Future improvements: load a DBC file with signal definitions to work with any car

## Supported Signals

* Accelerator Pedal Position
* Brake Pedal Position
* Clutch Engaged
* Steering Wheel Angle
* E-brake Engaged

The CAN signals currently setup have been tested on:

* 2014 Subaru BRZ
* 2015 Subaru Forester

It should work with any modern Subaru. If you have a different vehicle, you'll need to reverse engineer the CAN signals and add them to `CANProcessor.h`

## Setup

This is a PlatformIO project and requires it to run. You can setup PlatformIO in VSCode or Atom pretty easily. More information at [PlatformIO](https://platformio.org)

For Xbox mode, you will need to have an MCP4261 digital potentiometer connected to actuate the triggers on the Xbox Adaptive Controller.

1. Upload the code to your Macchina M2: `pio run -t upload`
2. Use Button2 on the Macchina to set the device to either Xbox or PC emulation mode (green light for Xbox, blue light for PC)

For different cars, you'll need to adjust the CAN signals in `CANProcessor.cpp` and the steering range in `main.cpp`
