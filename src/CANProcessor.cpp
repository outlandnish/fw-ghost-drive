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
    SerialUSB.print("ID: 0x");
    SerialUSB.print(frame.id, HEX);
    SerialUSB.print(" Len: ");
    SerialUSB.print(frame.length);
    SerialUSB.print(" Data: 0x");
    for (int count = 0; count < frame.length; count++) {
        SerialUSB.print(frame.data.bytes[count], HEX);
        SerialUSB.print(" ");
    }
    SerialUSB.print("\r\n");

    SerialUSB.print("Steering: ");
    SerialUSB.println(steering);  

    if (steering != pose.steering) {
      newData = true;
      pose.steering = steering;
    }
  }
  else if (frame.id == 0xD1) {
    // brake pedal pressure
    uint8_t brakes = frame.data.bytes[2];

    SerialUSB.print("Brakes: ");
    SerialUSB.println(brakes);

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

    //SerialUSB.print("Accelerator: ");
    //SerialUSB.println(accelerator);
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

  return newData;
}