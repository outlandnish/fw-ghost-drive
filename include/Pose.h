#pragma once
#include <stdint.h>
#include <Gear.h>

struct Pose {
  int16_t steering;
  uint8_t brakes;
  uint8_t accelerator;
  bool clutch;
  bool upshift;
  bool downshift;
  bool ebrake;
  bool rewind;
  bool view;
  bool menu;
  bool inGear;
  bool inReverse;
  Gear gear = Gear::Neutral;
};