#pragma once
#include <stdint.h>

enum Gear : uint8_t {
  Neutral = 0,
  First,
  Second,
  Third,
  Fourth,
  Fifth,
  Sixth,
  Reverse
};