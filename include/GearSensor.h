#pragma once
#include <Arduino.h>
#include <Pose.h>
#include <Gear.h>
// #define LOCAL_SENSOR

#if defined(LOCAL_SENSOR)
  #include <Adafruit_FXOS8700.h>
#endif;

#define NUM_GEAR_UPDATES 3

class GearSensor {
  char buffer;
  Gear gear = Gear::Neutral;

#if defined(LOCAL_SENSOR)
  Adafruit_FXOS8700 fxos = Adafruit_FXOS8700(0x8700A, 0x8700B);
  Adafruit_Sensor *accelerometer;
  sensors_event_t gravity;
  float _alpha = 0.05;

  uint8_t index = 0;
  Gear updates[NUM_GEAR_UPDATES];
#endif

  public:
    bool setup();
    void process();

    Gear getGear() { return gear; }
};