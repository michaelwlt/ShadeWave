#include "VentController.h"
#include "SensorData.h"
#include "Config.h"

namespace ShadeWave {
namespace Controller {

VentController::VentController(int servoPin)
  : ventOpen(false),
    servoPin(servoPin) {
}

void VentController::initialize() {
  // Initialize servo motor
  servoMotor.attach(servoPin);
  testServo();
}

void VentController::updateServoPosition(bool open) {
  int angle = open ? ShadeWave::Config::VENT_SERVO_OPEN_ANGLE : ShadeWave::Config::VENT_SERVO_CLOSED_ANGLE;
  servoMotor.write(angle);
}

void VentController::update(const Sensor::SensorData& sensors) {
  if (!sensors.getRoomOccupied()) {
    // Energy saving mode: Close vents
    ventOpen = false;
    updateServoPosition(false);
    return;
  }
  
  // Room is occupied - check if cooling is needed and possible
  if (sensors.getIsHotInside() && sensors.getOutdoorTemp() < sensors.getIndoorTemp()) {
    // Hot inside and cooler outside - consider natural cooling
    if (sensors.getIndoorHumidity() > sensors.getOutdoorHumidity()) {
      // Higher humidity inside - open vent to ventilate
      ventOpen = true;
      updateServoPosition(true);
    } else {
      // Lower/equal humidity inside - keep closed
      ventOpen = false;
      updateServoPosition(false);
    }
  } else {
    // Not hot inside OR warmer/equal outside - no cooling needed
    ventOpen = false;
    updateServoPosition(false);
  }
}

void VentController::testServo() {
  // Sweep servo from closed to open and back for testing
  servoMotor.write(ShadeWave::Config::VENT_SERVO_OPEN_ANGLE);
  delay(1000);
  servoMotor.write(ShadeWave::Config::VENT_SERVO_CLOSED_ANGLE);
  delay(500);
}

} // namespace Controller
} // namespace ShadeWave
