#include "IndoorSensor.h"

namespace ShadeWave {
namespace Sensor {

IndoorSensor::IndoorSensor(uint8_t pin)
  : oneWire(pin),
    sensors(&oneWire),
    temperature(0.0),
    ready(false) {
}

bool IndoorSensor::begin() {
  sensors.begin();
  
  // Check if any DS18B20 devices are found on the bus
  if (sensors.getDeviceCount() == 0) {
    ready = false;
    return false;
  }
  
  // Set resolution to 12-bit for highest accuracy
  sensors.setResolution(12);
  
  // Try a test read to verify sensor is working
  sensors.requestTemperatures();
  float testTemp = sensors.getTempCByIndex(0);
  
  // Check if we got a valid reading (returns DEVICE_DISCONNECTED_C on error)
  if (testTemp != DEVICE_DISCONNECTED_C) {
    ready = true;
    temperature = testTemp;
    return true;
  }
  
  ready = false;
  return false;
}

bool IndoorSensor::read() {
  if (!ready) {
    return false;
  }
  
  // Request temperature conversion
  sensors.requestTemperatures();
  float newTemp = sensors.getTempCByIndex(0);
  
  // Validate reading
  if (newTemp != DEVICE_DISCONNECTED_C) {
    temperature = newTemp;
    return true;
  }
  
  return false;
}

} // namespace Sensor
} // namespace ShadeWave
