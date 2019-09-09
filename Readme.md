# M2 HID / Xbox Controller

This firmware lets the Macchina M2 act as an HID Joystick using data from a car's CAN bus. When paired with an Xbox Adaptive Controller, it let's you play Xbox or PC games using your car!

The project is setup to work with a Subaru BRZ. Future improvements: load a DBC file with signal definitions to work with any car

## Supported Signals

* Accelerator Pedal Position
* Brake Pedal Position
* Clutch Engaged
* Steering Wheel Angle
* E-brake Engaged

Future improvements: custom device to get gear or use cruise control stalk as gear up/down

## Setup

This is a PlatformIO project and requires it to run. You can setup PlatformIO in VSCode or Atom pretty easily. More information at [PlatformIO](https://platformio.org)

For Xbox mode, you will need to have an MCP4261 digital potentiometer connected to actuate the triggers on the Xbox Adaptive Controller.

1. Upload the code to your Macchina M2: `pio run -t upload`
2. Use Button2 on the Macchina to set the device to either Xbox or PC emulation mode (green light for Xbox, blue light for PC)

For different cars, you'll need to adjust the CAN signals in `CANProcessor.cpp` and the steering range in `main.cpp`
