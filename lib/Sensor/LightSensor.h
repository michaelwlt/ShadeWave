#ifndef LIGHT_SENSOR_H
#define LIGHT_SENSOR_H

#include <Arduino.h>

namespace ShadeWave {
namespace Sensor {

class LightSensor {
private:
  uint8_t pin;
  bool lightIntense;
  int lastReading;

public:
  LightSensor(uint8_t pin);
  
  // Initialize the LDR light sensor
  bool begin();
  
  // Read light level from sensor
  // Returns true if read was successful
  bool read();
  
  // Getter for last reading result
  bool isLightIntense() const { return lightIntense; }
  
  // Get the raw analog reading (0-1023)
  int getRawReading() const { return lastReading; }
  
  // Check if sensor is ready (LDR needs no warm-up)
  bool isReady() const { return true; }
};

} // namespace Sensor
} // namespace ShadeWave

#endif // LIGHT_SENSOR_H
