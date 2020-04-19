#include <GearSensor.h>

bool GearSensor::setup() {
#if defined(LOCAL_SENSOR)
  if (!fxos.begin()) {
    Serial.println("Failed to find sensor");
    while (1) delay(10);
  }

  for (int i = 0; i < NUM_GEAR_UPDATES; i++)
    updates[i] = Gear::Neutral;

  accelerometer = fxos.getAccelerometerSensor();
#else
  Serial3.begin(115200);
#endif
}

void GearSensor::process() {
#if defined(LOCAL_SENSOR)
  sensors_event_t accel;
  accelerometer->getEvent(&accel);

  Gear current;

  gravity.acceleration.x = _alpha * gravity.acceleration.x + (1 - _alpha) * accel.acceleration.x;
  gravity.acceleration.y = _alpha * gravity.acceleration.y + (1 - _alpha) * accel.acceleration.y;
  gravity.acceleration.z = _alpha * gravity.acceleration.z + (1 - _alpha) * accel.acceleration.z;

  if (gravity.acceleration.y <= -3) {
    if (gravity.acceleration.x <= -0.80)
      current = Gear::Second;
    else if (gravity.acceleration.x <= 0.5)
      current = Gear::Fourth;
    else
      current = Gear::Sixth;
  }

  else if (gravity.acceleration.y >= -1.5) {
    if (gravity.acceleration.x <= -1)
      current = Gear::First;
    else if (gravity.acceleration.x <= 0.25)
      current = Gear::Third;
    else
      current = Gear::Fifth;
  }

  else
    current = Gear::Neutral;
  
  updates[index] = current;
  index++;
  index %= NUM_GEAR_UPDATES;

  bool stable = true;
  for (int i = 0; i++; i < NUM_GEAR_UPDATES)
    if (updates[i] != current) {
      stable = false;
      break;
    }
  
  if (stable && current != gear) {
    gear = current;
  }
#else
  while (Serial3.available()) {
    buffer = Serial3.read();
    gear = (Gear)(buffer - '0');
  }
#endif
}