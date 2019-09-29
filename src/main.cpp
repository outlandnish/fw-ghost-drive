#include <main.h>

void setup() {
  // start serial
  SerialUSB.begin(115200);

  delay(50);
  SerialUSB.println("Initializing...");
  
  // setup the M2 LEDs and buttons
  setupLightsAndButtons();

  // setup HID Joystick
  setupJoystick();

  // start SPI communication to the MCP4261
  //setupPotentiometers();

  // setup CAN
  setupCAN();

  // change mode when button is pressed
  //attachInterrupt(Button2, toggleEmulationMode, FALLING);

  SerialUSB.println("Ready");
}

void loop() {
  if (can.newVehicleData()) {
    updatePose(can.pose);
  }

  // Pose pose;
  // pose.accelerator = rand() % 255;
  // pose.brakes = rand() % 60;
  // pose.steering = rand() % 11200 - 5600;
  // updatePose(pose);
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
  joystick = new Joystick_(JOYSTICK_DEFAULT_REPORT_ID, JOYSTICK_TYPE_GAMEPAD, 8, 0, true, true, true, false, false, false, false, false, false, false, false);
  joystick->setXAxisRange(-STEERING_RANGE, STEERING_RANGE);
  joystick->setYAxisRange(0, ACCEL_MAX);
  joystick->setZAxisRange(0, BRAKE_MAX);
  joystick->begin(false);
}

void setupPotentiometers() {
  SPI.begin();
  pot.setResistance(0, 0);
  pot.setResistance(1, 0);
  pot.scale = 255.0;
}

void setupCAN() {
  digitalWrite(DS6, LOW);
  SerialUSB.println("Doing Auto Baud scan on CAN0");
  Can0.beginAutoSpeed();
  SerialUSB.println("Doing Auto Baud scan on CAN1");
  Can1.beginAutoSpeed();
  digitalWrite(DS7_BLUE, LOW);

  int filter;
  //extended
  for (filter = 0; filter < 3; filter++) {
    Can0.setRXFilter(filter, 0, 0, true);
    Can1.setRXFilter(filter, 0, 0, true);
  }  
  //standard
  for (int filter = 3; filter < 7; filter++) {
    Can0.setRXFilter(filter, 0, 0, false);
    Can1.setRXFilter(filter, 0, 0, false);
  }
}

void updatePose(Pose pose) {
  SerialUSB.println(pose.steering);
  switch (mode) {
    case EmulationMode::Xbox:
      float scaledAccel = accelScaler * (float)pose.accelerator;
      float scaledBrakes = brakeScaler * (float)pose.brakes;

      // port 1 = left trigger
      pot.setResistance(1, scaledBrakes);

      // port 0 = right trigger
      pot.setResistance(0, scaledAccel);

      // steering
      joystick->setXAxis(pose.steering);
      break;
    case EmulationMode::PC:
      // steering, accelerator, and brakes
      joystick->setXAxis(pose.steering);

      joystick->setYAxis(ACCEL_MAX - pose.accelerator);
      joystick->setZAxis(pose.brakes);
      break;
  }

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