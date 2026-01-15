#include "OutdoorSensor.h"

namespace ShadeWave {
namespace Sensor {

OutdoorSensor::OutdoorSensor(uint8_t pin)
  : dht(pin, DHT22),
    temperature(0.0),
    humidity(0.0),
    ready(false) {
}

bool OutdoorSensor::begin() {
  dht.begin();
  delay(2000);  // DHT22 needs 2 seconds to stabilize after power-on
  
  // Try a test read to verify sensor is connected
  float testTemp = dht.readTemperature();
  float testHumidity = dht.readHumidity();
  
  // Check if we got valid readings (DHT returns NaN on error)
  if (!isnan(testTemp) && !isnan(testHumidity)) {
    ready = true;
    temperature = testTemp;
    humidity = testHumidity;
    return true;
  }
  
  ready = false;
  return false;
}

bool OutdoorSensor::read() {
  if (!ready) {
    return false;
  }
  
  float newTemp = dht.readTemperature();
  float newHumidity = dht.readHumidity();
  
  // Validate readings (DHT returns NaN on error)
  if (!isnan(newTemp) && !isnan(newHumidity)) {
    temperature = newTemp;
    humidity = newHumidity;
    return true;
  }
  
  return false;
}

} // namespace Sensor
} // namespace ShadeWave
