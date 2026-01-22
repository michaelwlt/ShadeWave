#ifndef BLINDS_CONTROLLER_H
#define BLINDS_CONTROLLER_H

#include <Arduino.h>
#include <Servo.h>

namespace ShadeWave {
namespace Sensor {
  class SensorData;
}

namespace Controller {

enum class BlindsState {
  OPEN,
  CLOSED,
  NEUTRAL
};

class BlindsController {
private:
  BlindsState blindsState;
  int servoPin;
  int servoPin2;           
  Servo servoMotor;
  Servo servoMotor2;       
  
  void updateServoPosition(BlindsState state);

public:
  BlindsController(int servoPin, int servoPin2);
  
  void initialize();
  void update(const Sensor::SensorData& sensors);
  void testServo();  // Test sweep for startup verification
  
  BlindsState getState() const { return blindsState; }
};

} // namespace Controller
} // namespace ShadeWave

#endif // BLINDS_CONTROLLER_H
