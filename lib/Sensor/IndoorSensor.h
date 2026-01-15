#ifndef INDOOR_SENSOR_H
#define INDOOR_SENSOR_H

#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

namespace ShadeWave {
namespace Sensor {

class IndoorSensor {
private:
  OneWire oneWire;
  DallasTemperature sensors;
  float temperature;
  bool ready;

public:
  IndoorSensor(uint8_t pin);
  
  // Initialize the DS18B20 sensor
  bool begin();
  
  // Read temperature from sensor
  // Returns true if read was successful
  bool read();
  
  // Getter for last reading
  float getTemperature() const { return temperature; }
  
  // Check if sensor is connected and ready
  bool isReady() const { return ready; }
};

} // namespace Sensor
} // namespace ShadeWave

#endif // INDOOR_SENSOR_H
