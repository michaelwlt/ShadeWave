#include "MotionSensor.h"
#include <Config.h>

namespace ShadeWave {
namespace Sensor {

MotionSensor::MotionSensor(uint8_t pin)
  : pin(pin),
    motionDetected(false),
    initTime(0) {
}

bool MotionSensor::begin() {
  // Configure pin as input for reading PIR sensor output
  pinMode(pin, INPUT);
  
  // Record initialization time for warm-up period tracking
  // PIR sensors typically need 10-60 seconds to stabilize after power-on
  initTime = millis();
  
  return true;
}

bool MotionSensor::read() {
  if (!isReady()) {
    return false;
  }
  
  // PIR sensor outputs HIGH when motion is detected
  motionDetected = digitalRead(pin) == HIGH;
  
  return true;
}

bool MotionSensor::isReady() const {
  // Sensor is ready once the warm-up period has elapsed
  return (millis() - initTime) >= Config::PIR_WARMUP_MS;
}

} // namespace Sensor
} // namespace ShadeWave
