#include <main.h>

void setup() {
  // setup HID Joystick
  setupJoystick();

  // start serial
  SerialUSB.begin(115200);
  SerialUSB.println("Initializing...");
  
  // setup the M2 LEDs and buttons
  setupLightsAndButtons();

  // start SPI communication to the MCP4261
  // setupPotentiometers();

  // setup CAN
  setupCAN();

  // change mode when button is pressed
  attachInterrupt(Button2, toggleEmulationMode, FALLING);

  gearSensor.setup();

  SerialUSB.println("Ready");
}

void loop() {
  gearSensor.process();
  Gear gear = gearSensor.getGear();

  // if we have any new CAN bus data
  if (can.newVehicleData() || gear != lastGear) {
    Pose pose = can.pose;
    pose.gear = gearSensor.getGear();

    updatePose(pose);
    lastPose = pose;
  }

  // if we have any telemetry updates to make
  // if (SerialUSB.available())
    // processTelemetry(SerialUSB.readStringUntil('\n').c_str());

  // can.updateDashboard();
}

void setupLightsAndButtons() {
  pinMode(DS2, OUTPUT);
  pinMode(DS3, OUTPUT);
  pinMode(DS4, OUTPUT);
  pinMode(DS5, OUTPUT);
  pinMode(DS6, OUTPUT);
  pinMode(DS7_RED, OUTPUT);
  pinMode(DS7_GREEN, OUTPUT);
  pinMode(DS7_BLUE, OUTPUT);

  pinMode(Button1, INPUT_PULLUP);
  pinMode(Button2, INPUT_PULLUP);

  digitalWrite(DS2, HIGH);
  digitalWrite(DS3, HIGH);
  digitalWrite(DS4, HIGH);
  digitalWrite(DS5, HIGH);
  digitalWrite(DS6, HIGH);
  digitalWrite(DS7_RED, HIGH);
  digitalWrite(DS7_GREEN, LOW);
  digitalWrite(DS7_BLUE, HIGH);
}

void setupJoystick() {
  joystick = new Joystick_(JOYSTICK_DEFAULT_REPORT_ID, JOYSTICK_TYPE_GAMEPAD, 10, 0, true, true, true, false, false, false, false, false, false, false, false);
  joystick->setXAxisRange(-STEERING_RANGE, STEERING_RANGE);
  joystick->setYAxisRange(0, ACCEL_MAX);
  joystick->setZAxisRange(0, BRAKE_MAX);
  joystick->begin(false);
  joystick->pressButton(buttonForGear(lastGear));
}

void setupPotentiometers() {
  SPI.begin();
  pot.setResistance(0, 0);
  pot.setResistance(1, 0);
  pot.scale = 255.0;
}

void setupCAN() {
  digitalWrite(DS6, LOW);
  Can0.begin(500000);
  Can1.begin(500000);
  digitalWrite(DS7_BLUE, LOW);

  int filter;
  Can0.setRXFilter(0, 0xD0, 0, false);
  Can0.setRXFilter(1, 0xD1, 0, false);
  Can0.setRXFilter(2, 0x140, 0, false);
  Can0.setRXFilter(3, 0x144, 0, false);
  Can0.setRXFilter(4, 0x152, 0, false);
}

void updatePose(Pose pose) {
  float scaledAccel, scaledBrakes;

  SerialUSB.print("Pose:\tAccel: ");
  SerialUSB.print(pose.accelerator);
  SerialUSB.print("\tBrakes: ");
  SerialUSB.print(pose.brakes);
  SerialUSB.print("\tSteering: ");
  SerialUSB.print(pose.steering);
  SerialUSB.print("\tClutch: ");
  SerialUSB.print(pose.clutch);
  SerialUSB.print("\tE-brake: ");
  SerialUSB.print(pose.ebrake);
  SerialUSB.print("\tGear: ");
  SerialUSB.print(pose.gear);
  SerialUSB.println("");

  switch (mode) {
    case EmulationMode::Xbox:
      scaledAccel = accelScaler * (float)pose.accelerator;
      scaledBrakes = brakeScaler * (float)pose.brakes;

      // port 1 = left trigger
      pot.setResistance(1, scaledBrakes);

      // port 0 = right trigger
      pot.setResistance(0, scaledAccel);

      // steering
      joystick->setXAxis(pose.steering);
      
      // clutch
      pose.clutch ? joystick->pressButton(XboxButtons::LEFT_BUMPER) : joystick->releaseButton(XboxButtons::LEFT_BUMPER);

      // e-brake
      pose.ebrake ? joystick->pressButton(XboxButtons::A) : joystick->releaseButton(XboxButtons::A);

      // upshift
      pose.upshift ? joystick->pressButton(XboxButtons::B) : joystick->releaseButton(XboxButtons::B);

      // downshift: remap X1 on Adaptive Controller to X
      pose.downshift ? joystick->pressButton(XboxButtons::LEFT_X1) : joystick->releaseButton(XboxButtons::LEFT_X1);

      // rewind: remap X2 on Adaptive Controller to Y
      pose.rewind ? joystick->pressButton(XboxButtons::LEFT_X2) : joystick->releaseButton(XboxButtons::LEFT_X2);
      break;
    case EmulationMode::PC:
      // steering, accelerator, and brakes
      joystick->setXAxis(pose.steering);
      joystick->setYAxis(ACCEL_MAX - pose.accelerator);
      joystick->setZAxis(pose.brakes);

      // gear
      uint8_t button = buttonForGear(pose.gear);
      if (pose.gear != lastGear) {
        joystick->releaseButton(buttonForGear(lastGear));
        // press and store new gear
        joystick->pressButton(button);
        lastGear = pose.gear;
      }

      // upshift pressed
      // pose.upshift ? joystick->pressButton(0) : joystick->releaseButton(0);

      // downshift pressed
      // pose.downshift ? joystick->pressButton(1) : joystick->releaseButton(1);

      // clutch
      pose.clutch ? joystick->pressButton(8) : joystick->releaseButton(8);

      // e-brake
      pose.ebrake ? joystick->pressButton(9) : joystick->releaseButton(9);
      
      break;
  }

  // update joystick state
  joystick->sendState();
}

void toggleEmulationMode() {
  switch (mode) {
    case EmulationMode::Xbox:
      mode = EmulationMode::PC;
      digitalWrite(DS7_GREEN, LOW);
      digitalWrite(DS7_RED, HIGH);
      break;
    case EmulationMode::PC:
      mode = EmulationMode::Xbox;
      digitalWrite(DS7_GREEN, HIGH);
      digitalWrite(DS7_RED, LOW);
      break;
  }
}

void processTelemetry(std::string data) {
  digitalWrite(DS5, LOW);
  std::vector<std::string> parts;
  tokenize(data, ',', parts);
  can.updateTelemetry(std::stof(parts[0]), std::stoi(parts[1]));
  digitalWrite(DS5, HIGH);
}

uint8_t buttonForGear(Gear gear) {
  return gear == gear == Gear::Reverse ? 8 : gear;
}