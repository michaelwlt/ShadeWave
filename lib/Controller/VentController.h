#ifndef VENT_CONTROLLER_H
#define VENT_CONTROLLER_H

#include <Arduino.h>
#include <Servo.h>

namespace ShadeWave {
namespace Sensor {
  class SensorData;
}

namespace Controller {

class VentController {
private:
  bool ventOpen;
  int openPin;
  int closedPin;
  int servoPin;
  Servo servoMotor;
  
  void updateVentOutputs(bool open);

public:
  VentController(int openPin, int closedPin, int servoPin);
  
  void initialize();
  void update(const Sensor::SensorData& sensors);
  void testServo();  // Test sweep for startup verification
  
  bool isOpen() const { return ventOpen; }
};

} // namespace Controller
} // namespace ShadeWave

#endif // VENT_CONTROLLER_H
