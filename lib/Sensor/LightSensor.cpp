#include "LightSensor.h"
#include <Config.h>

namespace ShadeWave {
namespace Sensor {

LightSensor::LightSensor(uint8_t pin)
  : pin(pin),
    lightIntense(false),
    lastReading(0) {
}

bool LightSensor::begin() {
  // Analog pins don't require explicit pinMode configuration on Arduino
  // They are automatically configured when analogRead() is called
  return read();
}

bool LightSensor::read() {
  // Read analog value from LDR (0-1023 range)
  lastReading = analogRead(pin);
  
  // Determine if light is intense based on threshold
  // LDR has inverse relationship: lower reading = more light (lower resistance)
  lightIntense = (lastReading <= Config::LDR_INTENSE_THRESHOLD);
  
  // Analog reads always succeed on Arduino
  return true;
}

} // namespace Sensor
} // namespace ShadeWave
