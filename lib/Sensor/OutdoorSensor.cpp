#include "OutdoorSensor.h"

namespace ShadeWave {
namespace Sensor {

OutdoorSensor::OutdoorSensor()
  : temperature(0.0),
    humidity(0.0),
    ready(false) {
}

bool OutdoorSensor::begin() {
  sht20.initSHT20();
  delay(100);  // Give sensor time to initialize
  
  // Try a test read to verify sensor is connected
  float testTemp = sht20.readTemperature();
  
  // Check if we got a valid reading (SHT20 returns very large negative values on error)
  if (testTemp > -40.0 && testTemp < 125.0) {
    ready = true;
    temperature = testTemp;
    humidity = sht20.readHumidity();
    return true;
  }
  
  ready = false;
  return false;
}

bool OutdoorSensor::read() {
  if (!ready) {
    return false;
  }
  
  float newTemp = sht20.readTemperature();
  float newHumidity = sht20.readHumidity();
  
  // Validate readings are within sensor's operating range
  if (newTemp >= -40.0 && newTemp <= 125.0 &&
      newHumidity >= 0.0 && newHumidity <= 100.0) {
    temperature = newTemp;
    humidity = newHumidity;
    return true;
  }
  
  return false;
}

} // namespace Sensor
} // namespace ShadeWave
