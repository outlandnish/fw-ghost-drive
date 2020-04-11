#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_VL6180X.h>
#include <Pose.h>
#include <Gear.h>

#define ODD_RESET TXD3
#define EVEN_RESET RXD3

#define EVEN_ADDRESS VL6180X_DEFAULT_I2C_ADDR
#define ODDS_ADDRESS (VL6180X_DEFAULT_I2C_ADDR + 1)

class GearSensor {
  Adafruit_VL6180X odds = Adafruit_VL6180X();
  Adafruit_VL6180X evens = Adafruit_VL6180X();
  uint32_t lastGearUpdate = millis();
  uint32_t GEAR_UPDATE_DELTA = 50;  // milliseconds

  uint8_t oddsValue, oddsStatus;
  uint8_t evensValue, evensStatus;

  public:
    bool setup();
    void process();

    Gear calculateGear(Pose pose);
};