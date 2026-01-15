#ifndef MOTION_SENSOR_H
#define MOTION_SENSOR_H

#include <Arduino.h>

namespace ShadeWave {
namespace Sensor {

class MotionSensor {
private:
  uint8_t pin;
  bool motionDetected;
  unsigned long initTime;  // Timestamp when begin() was called

public:
  MotionSensor(uint8_t pin);
  
  // Initialize the PIR motion sensor
  bool begin();
  
  // Read motion state from sensor
  // Returns true if read was successful
  bool read();
  
  // Getter for last reading
  bool isMotionDetected() const { return motionDetected; }
  
  // Check if sensor is ready (warm-up period has elapsed)
  bool isReady() const;
};

} // namespace Sensor
} // namespace ShadeWave

#endif // MOTION_SENSOR_H
