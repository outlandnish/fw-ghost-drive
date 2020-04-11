#include <Arduino.h>
#undef min
#undef max
#include <SPI.h>
#include <McpDigitalPot.h>
#include <CANProcessor.h>
#include <Joystick.h>
#include <Pose.h>
#include <XboxButtons.h>
#include <EmulationMode.h>
#include <GearSensor.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

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

// Gear sensing
GearSensor gearSensor;

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
void processTelemetry(std::string data);
void toggleEmulationMode();

void tokenize(const std::string& s, const char delim, std::vector<std::string>& out)
{
	std::string::size_type beg = 0;
	for (auto end = 0; (end = s.find(delim, end)) != std::string::npos; ++end)
	{
		out.push_back(s.substr(beg, end - beg));
		beg = end + 1;
	}

	out.push_back(s.substr(beg));
}