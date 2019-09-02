#include <CANProcessor.h>

bool CANProcessor::newVehicleData() {
  bool newData = false;
  newData = newData || checkBus(&Can0);
  newData = newData || checkBus(&Can1);

  return newData;
}

bool CANProcessor::checkBus(CANRaw *bus) {
  CAN_FRAME frame;
  if (bus->available() > 0) {
    bus->read(frame);
    processFrame(frame);
  }
}

bool CANProcessor::processFrame(CAN_FRAME &frame) {
  bool newData = false;
  if (frame.id == 0xD0) {
    // steering
    int16_t steering = frame.data.bytes[1] << 8 | frame.data.bytes[0];

    if (steering != pose.steering) {
      newData = true;
      pose.steering = steering;
    }
  }
  else if (frame.id == 0xD1) {
    // brake pedal pressure
    uint8_t brakes = frame.data.bytes[2];

    if (brakes != pose.brakes) {
      newData = true;
      pose.brakes = brakes;
    }
  }
  else if (frame.id == 0x140) {
    // accelerator pedal position + clutch engaged / not
    uint8_t accelerator = frame.data.bytes[0];
    bool clutch = frame.data.bit[15];

    if (pose.accelerator != accelerator || pose.clutch != clutch)
      newData = true;
    
    pose.accelerator = accelerator;
    pose.clutch = clutch;
  }
  else if (frame.id == 0x141) {
    // TODO: transmission (we can only see if we are in gear for manual BRZs)
  }
  else if (frame.id == 0x144) {
    // TODO: use cruise control stalk up/down as replacement for gear shifts?
  }
  else if (frame.id == 0x152) {
    // e-brake engaged or not
    bool ebrake = frame.data.bit[51];

    if (pose.ebrake != ebrake)
      newData = true;

    pose.ebrake = ebrake;
  }
}