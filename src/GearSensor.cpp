#include <GearSensor.h>

bool GearSensor::setup() {
  pinMode(ODD_RESET, OUTPUT);
  pinMode(EVEN_RESET, OUTPUT);

  // shutdown both VLX6180s
  digitalWrite(ODD_RESET, LOW);
  digitalWrite(EVEN_RESET, LOW);
  delay(1000);

  // turn on first VLX6180 - (1st / 3rd / 5th)
  digitalWrite(ODD_RESET, HIGH);
  delay(10);

  // if we failed to startup the sensor
  if (!odds.begin(&Wire1))
    return false;

  // change I2C address so it doesn't interfere with the other sensor
  odds.setAddress(ODDS_ADDRESS);
  delay(50);

  // if we fail to connect after changing address
  if (!odds.begin(&Wire1, ODDS_ADDRESS))
    return false;

  // turn on second VLX6180 - (2nd / 4th / 6th)
  digitalWrite(EVEN_RESET, HIGH);
  delay(10);

  if (!evens.begin(&Wire1))
    return false;

  return true;
}

void GearSensor::process() {  
  if (millis() - lastGearUpdate >= GEAR_UPDATE_DELTA) {
    evensValue = evens.readRange();
    evensStatus = evens.readRangeStatus();

    oddsValue = odds.readRange();
    oddsStatus = odds.readRangeStatus();

    lastGearUpdate = millis();
  }
}

Gear GearSensor::calculateGear(Pose pose) {
  if (pose.inReverse)  return Gear::Reverse;
  else if (!pose.inGear) return Gear::Neutral;

  if (evensStatus == VL6180X_ERROR_NONE) {

  }

  if (oddsStatus == VL6180X_ERROR_NONE) {

  }

  // default
  return Gear::Neutral;
}