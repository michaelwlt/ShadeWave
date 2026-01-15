#ifndef MOTION_SENSOR_H
#define MOTION_SENSOR_H

#include <Arduino.h>

namespace ShadeWave {
namespace Sensor {

class MotionSensor {
private:
  uint8_t pin;
  bool motionDetected;
  bool ready;

public:
  MotionSensor(uint8_t pin);
  
  // Initialize the PIR motion sensor
  bool begin();
  
  // Read motion state from sensor
  // Returns true if read was successful
  bool read();
  
  // Getter for last reading
  bool isMotionDetected() const { return motionDetected; }
  
  // Check if sensor is ready
  bool isReady() const { return ready; }
};

} // namespace Sensor
} // namespace ShadeWave

#endif // MOTION_SENSOR_H
