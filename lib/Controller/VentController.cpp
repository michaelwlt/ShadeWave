#include "VentController.h"
#include "SensorData.h"
#include "Config.h"

namespace ShadeWave {
namespace Controller {

VentController::VentController(int openPin, int closedPin, int servoPin)
  : ventOpen(false),
    openPin(openPin),
    closedPin(closedPin),
    servoPin(servoPin) {
}

void VentController::initialize() {
  pinMode(openPin, OUTPUT);
  pinMode(closedPin, OUTPUT);
  digitalWrite(openPin, LOW);
  digitalWrite(closedPin, LOW);
  
  // Initialize servo motor
  servoMotor.attach(servoPin);
  servoMotor.write(ShadeWave::Config::VENT_SERVO_CLOSED_ANGLE);
}

void VentController::updateVentOutputs(bool open) {
  // Update LED indicators
  digitalWrite(openPin, open ? HIGH : LOW);
  digitalWrite(closedPin, open ? LOW : HIGH);
  
  // Update servo position
  int angle = open ? ShadeWave::Config::VENT_SERVO_OPEN_ANGLE : ShadeWave::Config::VENT_SERVO_CLOSED_ANGLE;
  servoMotor.write(angle);
}

void VentController::update(const Sensor::SensorData& sensors) {
  if (!sensors.getRoomOccupied()) {
    // Energy saving mode: Close vents
    ventOpen = false;
    updateVentOutputs(false);
    return;
  }
  
  // Room is occupied - compare temperatures
  if (sensors.getOutdoorTemp() < sensors.getIndoorTemp()) {
    // Cooler outside
    if (sensors.getOutdoorHumidity() > sensors.getHumidityThreshold()) {
      // High humidity outside
      ventOpen = false;
      updateVentOutputs(false);
    } else {
      // Low humidity - natural cooling
      ventOpen = true;
      updateVentOutputs(true);
    }
  } else {
    // Warmer outside
    ventOpen = false;
    updateVentOutputs(false);
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
