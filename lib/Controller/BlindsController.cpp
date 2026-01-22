#include "BlindsController.h"
#include "SensorData.h"
#include "Config.h"

namespace ShadeWave {
namespace Controller {

BlindsController::BlindsController(int servoPin)
  : blindsState(BlindsState::NEUTRAL),
    servoPin(servoPin) {
}

void BlindsController::initialize() {
  servoMotor.attach(servoPin);
  servoMotor.write(ShadeWave::Config::BLINDS_SERVO_NEUTRAL_ANGLE);
}

void BlindsController::updateServoPosition(BlindsState state) {
  switch(state) {
    case BlindsState::OPEN:
      servoMotor.write(ShadeWave::Config::BLINDS_SERVO_OPEN_ANGLE);
      break;
    case BlindsState::CLOSED:
      servoMotor.write(ShadeWave::Config::BLINDS_SERVO_CLOSED_ANGLE);
      break;
    case BlindsState::NEUTRAL:
      servoMotor.write(ShadeWave::Config::BLINDS_SERVO_NEUTRAL_ANGLE);
      break;
  }
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
  // Sweep servo through all three positions for testing
  servoMotor.write(ShadeWave::Config::BLINDS_SERVO_CLOSED_ANGLE);
  delay(500);
  servoMotor.write(ShadeWave::Config::BLINDS_SERVO_NEUTRAL_ANGLE);
  delay(500);
  servoMotor.write(ShadeWave::Config::BLINDS_SERVO_OPEN_ANGLE);
  delay(500);
  servoMotor.write(ShadeWave::Config::BLINDS_SERVO_NEUTRAL_ANGLE);
  delay(500);
}

} // namespace Controller
} // namespace ShadeWave
