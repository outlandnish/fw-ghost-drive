#include <due_can.h>
#include <Pose.h>

class CANProcessor {
  public:
    bool newVehicleData();

    bool checkBus(CANRaw *bus);
    bool processFrame(CAN_FRAME &frame);

    Pose pose;
};