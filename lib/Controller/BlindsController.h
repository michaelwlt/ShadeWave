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
  int openPin;
  int closedPin;
  int neutralPin;
  int servoPin;
  Servo servoMotor;
  
  void updateBlindsLEDs(BlindsState state);

public:
  BlindsController(int openPin, int closedPin, int neutralPin, int servoPin);
  
  void initialize();
  void update(const Sensor::SensorData& sensors);
  void testServo();  // Test sweep for startup verification
  
  BlindsState getState() const { return blindsState; }
};

} // namespace Controller
} // namespace ShadeWave

#endif // BLINDS_CONTROLLER_H
