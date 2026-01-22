#include "BlindsController.h"
#include "SensorData.h"
#include "Config.h"

namespace ShadeWave {
namespace Controller {

BlindsController::BlindsController(int servoPin, int servoPin2)
  : blindsState(BlindsState::NEUTRAL),
    servoPin(servoPin),
    servoPin2(servoPin2) {
}

void BlindsController::initialize() {
  servoMotor.attach(servoPin);
  servoMotor2.attach(servoPin2);
  testServo();
}

void BlindsController::updateServoPosition(BlindsState state) {
  int angle;
  switch(state) {
    case BlindsState::OPEN:
      angle = ShadeWave::Config::BLINDS_SERVO_OPEN_ANGLE;
      break;
    case BlindsState::CLOSED:
      angle = ShadeWave::Config::BLINDS_SERVO_CLOSED_ANGLE;
      break;
    case BlindsState::NEUTRAL:
      angle = ShadeWave::Config::BLINDS_SERVO_NEUTRAL_ANGLE;
      break;
  }
  servoMotor.write(angle);
  servoMotor2.write(180 - angle);  // Inverted angle for second servo
}

void BlindsController::update(const Sensor::SensorData& sensors) {
  if (!sensors.getRoomOccupied()) {
    // Energy saving mode: Close blinds
    blindsState = BlindsState::CLOSED;
    updateServoPosition(BlindsState::CLOSED);
    return;
  }
  
  // Room is occupied - check sunlight
  if (sensors.getSunlightIntense()) {
    if (sensors.getIsHotInside()) {
      // Block heat
      blindsState = BlindsState::CLOSED;
      updateServoPosition(BlindsState::CLOSED);
    } else {
      // Solar heating
      blindsState = BlindsState::OPEN;
      updateServoPosition(BlindsState::OPEN);
    }
  } else {
    // Sunlight not intense
    blindsState = BlindsState::NEUTRAL;
    updateServoPosition(BlindsState::NEUTRAL);
  }
}

void BlindsController::testServo() {
  // Sweep both servos through all three positions for testing
  // Servo2 moves with inverted angle (180 - angle)
  servoMotor.write(ShadeWave::Config::BLINDS_SERVO_CLOSED_ANGLE);
  servoMotor2.write(180 - ShadeWave::Config::BLINDS_SERVO_CLOSED_ANGLE);
  delay(500);
  servoMotor.write(ShadeWave::Config::BLINDS_SERVO_NEUTRAL_ANGLE);
  servoMotor2.write(180 - ShadeWave::Config::BLINDS_SERVO_NEUTRAL_ANGLE);
  delay(500);
  servoMotor.write(ShadeWave::Config::BLINDS_SERVO_OPEN_ANGLE);
  servoMotor2.write(180 - ShadeWave::Config::BLINDS_SERVO_OPEN_ANGLE);
  delay(500);
  servoMotor.write(ShadeWave::Config::BLINDS_SERVO_CLOSED_ANGLE);
  servoMotor2.write(180 - ShadeWave::Config::BLINDS_SERVO_CLOSED_ANGLE);
  delay(500);
}

} // namespace Controller
} // namespace ShadeWave
