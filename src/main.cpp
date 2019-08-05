#include <Arduino.h>
#include <due_can.h>

struct Pose {
  int16_t steering;
  uint8_t brakes;
  uint8_t accelerator;
  bool clutch;
  bool upshift;
  bool downshift;
  bool ebrake;
  bool rewind;
};

Pose pose;
bool newData = false;

void setup() {
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
  digitalWrite(DS7_GREEN, HIGH);
  digitalWrite(DS7_BLUE, HIGH);

  SerialUSB.begin(115200);
  Serial3.begin(9600);

  delay(50);
  SerialUSB.println("Initializing...");

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

void processFrame(CAN_FRAME &frame) {
  if (frame.id == 0xD0) {
    int16_t steering = frame.data.bytes[1] << 8 | frame.data.bytes[0];

    if (pose.steering != steering)
      newData = true;
      
    pose.steering = steering;
  }
  else if (frame.id == 0xD1) {
    uint8_t brakes = frame.data.bytes[2];

    if (pose.brakes != brakes)
      newData = true;

    pose.brakes = brakes;
  }
  else if (frame.id == 0x140) {
    uint8_t accelerator = frame.data.bytes[0];
    bool clutch = frame.data.bit[15];

    if (pose.accelerator != accelerator || pose.clutch != clutch)
      newData = true;
    
    pose.accelerator = accelerator;
    pose.clutch = clutch;
  }
  else if (frame.id == 0x141) {
    // TODO: transmission (we can only see if we are in gear)
  }
  else if (frame.id == 0x152) {
    bool ebrake = frame.data.bit[51];

    if (pose.ebrake != ebrake)
      newData = true;

    pose.ebrake = ebrake;
  }
}

void loop() {
  CAN_FRAME incoming;

  if (Can0.available() > 0) {
    digitalWrite(DS3, LOW);
    Can0.read(incoming); 
    processFrame(incoming);
  }

  if (Can1.available() > 0) {
    digitalWrite(DS4, LOW);
    Can1.read(incoming);  
    processFrame(incoming);
  }

  bool pressed;
  pressed = !digitalRead(Button1);
  digitalWrite(DS5, !pressed);
  
  if (pressed != pose.ebrake) {
    newData = true;
    pose.ebrake = pressed;
  }

  if (newData) {
    Serial3.print(pose.steering);
    Serial3.print(",");
    Serial3.print(pose.accelerator);
    Serial3.print(",");
    Serial3.print(pose.brakes);
    Serial3.print(",");
    Serial3.print(pose.clutch);
    Serial3.print(",");
    Serial3.print(pose.upshift);
    Serial3.print(",");
    Serial3.print(pose.downshift);
    Serial3.print(",");
    Serial3.print(pose.ebrake);
    Serial3.print(",");
    Serial3.print(pose.rewind);
    Serial3.print(",");
    Serial3.print(pose.ebrake);
    Serial3.print(",");
    Serial3.print(pose.ebrake);
    Serial3.print(",");
    Serial3.print(pose.ebrake);
    Serial3.print('\n');

    SerialUSB.print(pose.steering);
    SerialUSB.print(",");
    SerialUSB.print(pose.accelerator);
    SerialUSB.print(",");
    SerialUSB.print(pose.brakes);
    SerialUSB.print(",");
    SerialUSB.print(pose.clutch);
    SerialUSB.print(",");
    SerialUSB.print(pose.upshift);
    SerialUSB.print(",");
    SerialUSB.print(pose.downshift);
    SerialUSB.print(",");
    SerialUSB.print(pose.ebrake);
    SerialUSB.print(",");
    SerialUSB.print(pose.rewind);
    SerialUSB.print(",");
    SerialUSB.print(pose.ebrake);
    SerialUSB.print(",");
    SerialUSB.print(pose.ebrake);
    SerialUSB.print(",");
    SerialUSB.println(pose.ebrake);

    newData = false;
  }
}