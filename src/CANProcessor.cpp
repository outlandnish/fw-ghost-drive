#include <CANProcessor.h>

void CANProcessor::updateTelemetry(float speed, int rpm) {
  _speed = speed;
  _rpm = rpm;
}

void CANProcessor::updateDashboard() {
  union {
    uint16_t convertedSpeed;
    uint8_t bytes[2];
  } speedData;

  speedData.convertedSpeed = round(_speed / 0.05747);
  lastBrakeSpeedFrame.data.byte[0] = speedData.bytes[0];
  lastBrakeSpeedFrame.data.byte[1] = speedData.bytes[1];
  Can0.sendFrame(lastBrakeSpeedFrame);

  lastTransmissionFrame.data.byte[4] = _rpm % 256;
  lastTransmissionFrame.data.byte[5] = _rpm / 256 + 0x80;

  Can0.sendFrame(lastTransmissionFrame);
}

bool CANProcessor::newVehicleData() {
  bool newData = false;
  newData = newData || checkBus(&Can0);
  newData = newData || checkBus(&Can1);

  return newData;
}

bool CANProcessor::checkBus(CANRaw *bus) {
  CAN_FRAME frame;
  if (bus->available() > 0) {
    digitalWrite(DS2, LOW);

    bus->read(frame);
    return processFrame(frame);
  }
  digitalWrite(DS2, HIGH);
  return false;
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
    lastBrakeSpeedFrame = frame;

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
    lastTransmissionFrame = frame;
    // bool inGear = frame.data.bytes[5] == 0x80;

    // if (pose.inGear != inGear) {
    //   newData = true;
    //   pose.inGear = inGear;
    //   SerialUSB.print("In gear: ");
    //   SerialUSB.println(pose.inGear);
    // }
  }
  else if (frame.id == 0x144) {
    // cruise control stalk (used for upshift / downshift)
    uint8_t cruiseControlStalk = frame.data.byte[0];

    bool upshift = cruiseControlStalk == 0xD0;
    bool downshift = cruiseControlStalk == 0xC8;
    // if (pose.upshift != upshift || pose.downshift != downshift) {
    //   pose.upshift = upshift;
    //   pose.downshift = downshift;
    //   newData = true;
    // }
  }
  else if (frame.id == 0x152) {
    // e-brake engaged or not
    bool ebrake = frame.data.bit[51];

    if (pose.ebrake != ebrake)
      newData = true;

    pose.ebrake = ebrake;
  }

  return newData;
}