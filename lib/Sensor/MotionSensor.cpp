#include "MotionSensor.h"

namespace ShadeWave {
namespace Sensor {

MotionSensor::MotionSensor(uint8_t pin)
  : pin(pin),
    motionDetected(false),
    ready(false) {
}

bool MotionSensor::begin() {
  // Configure pin as input for reading PIR sensor output
  pinMode(pin, INPUT);
  
  // PIR sensors typically need 10-60 seconds to stabilize after power-on
  // For simulation purposes, we'll consider it ready immediately
  ready = true;
  
  // Do an initial read
  motionDetected = digitalRead(pin) == HIGH;
  
  return true;
}

bool MotionSensor::read() {
  if (!ready) {
    return false;
  }
  
  // PIR sensor outputs HIGH when motion is detected
  motionDetected = digitalRead(pin) == HIGH;
  
  return true;
}

} // namespace Sensor
} // namespace ShadeWave
