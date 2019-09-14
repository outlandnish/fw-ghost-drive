#include <Arduino.h>
#include <SPI.h>
#include <McpDigitalPot.h>
#include <CANProcessor.h>
#include <Joystick.h>
#include <Pose.h>
#include <XboxButtons.h>
#include <EmulationMode.h>

// digital potentiometer
float resistance = 9760.0; // 9.76 kohm - needs to be calibrated for each digital pot
McpDigitalPot pot = McpDigitalPot(SPI0_CS0, resistance);

// CAN interfaces
CANProcessor can;

// Joystick
Joystick_* joystick;

EmulationMode mode = EmulationMode::Xbox;

void setupLightsAndButtons();
void setupPotentiometers();
void setupCAN();
void setupJoystick();

void updatePose(Pose pose);
void toggleEmulationMode();