#include "BlindsController.h"
#include "SensorData.h"
#include "Config.h"

namespace ShadeWave {
namespace Controller {

BlindsController::BlindsController(int openPin, int closedPin, int neutralPin, int servoPin)
  : blindsState(BlindsState::NEUTRAL),
    openPin(openPin),
    closedPin(closedPin),
    neutralPin(neutralPin),
    servoPin(servoPin) {
}

void BlindsController::initialize() {
  pinMode(openPin, OUTPUT);
  pinMode(closedPin, OUTPUT);
  pinMode(neutralPin, OUTPUT);
  digitalWrite(openPin, LOW);
  digitalWrite(closedPin, LOW);
  digitalWrite(neutralPin, LOW);
  
  // Initialize servo motor
  servoMotor.attach(servoPin);
  servoMotor.write(ShadeWave::Config::BLINDS_SERVO_NEUTRAL_ANGLE);
}

void BlindsController::updateBlindsLEDs(BlindsState state) {
  // Turn off all blinds LEDs first
  digitalWrite(openPin, LOW);
  digitalWrite(closedPin, LOW);
  digitalWrite(neutralPin, LOW);
  
  // Turn on the appropriate LED and set servo position
  switch(state) {
    case BlindsState::OPEN:
      digitalWrite(openPin, HIGH);
      servoMotor.write(ShadeWave::Config::BLINDS_SERVO_OPEN_ANGLE);
      break;
    case BlindsState::CLOSED:
      digitalWrite(closedPin, HIGH);
      servoMotor.write(ShadeWave::Config::BLINDS_SERVO_CLOSED_ANGLE);
      break;
    case BlindsState::NEUTRAL:
      digitalWrite(neutralPin, HIGH);
      servoMotor.write(ShadeWave::Config::BLINDS_SERVO_NEUTRAL_ANGLE);
      break;
  }
}

void BlindsController::update(const Sensor::SensorData& sensors) {
  if (!sensors.getRoomOccupied()) {
    // Energy saving mode: Close blinds
    blindsState = BlindsState::CLOSED;
    updateBlindsLEDs(BlindsState::CLOSED);
    return;
  }
  
  // Room is occupied - check sunlight
  if (sensors.getSunlightIntense()) {
    if (sensors.getIsHotInside()) {
      // Summer mode: Block heat
      blindsState = BlindsState::CLOSED;
      updateBlindsLEDs(BlindsState::CLOSED);
    } else {
      // Winter mode: Solar heating
      blindsState = BlindsState::OPEN;
      updateBlindsLEDs(BlindsState::OPEN);
    }
  } else {
    // Sunlight not intense
    blindsState = BlindsState::NEUTRAL;
    updateBlindsLEDs(BlindsState::NEUTRAL);
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
