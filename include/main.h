#include <Arduino.h>
#include <SPI.h>
#include <McpDigitalPot.h>
#include <CANProcessor.h>
#include <Joystick.h>
#include <Pose.h>
#include <XboxButtons.h>
#include <EmulationMode.h>

#define STEERING_RANGE 4600
#define BRAKE_MAX 100
#define ACCEL_MAX 255

// digital potentiometer
float resistance = 9760.0; // 9.76 kohm - needs to be calibrated for each digital pot
McpDigitalPot pot = McpDigitalPot(SPI0_CS0, resistance);

float accelScaler = 255.0 / (float)ACCEL_MAX;
float brakeScaler = 255.0 / (float)BRAKE_MAX;

// CAN interfaces
CANProcessor can;

// Joystick
Joystick_* joystick;

EmulationMode mode = EmulationMode::Xbox;

// Gear
Gear lastGear;

void setupLightsAndButtons();
void setupPotentiometers();
void setupCAN();
void setupJoystick();

void updatePose(Pose pose);
void toggleEmulationMode();