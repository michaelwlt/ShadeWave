#ifndef OUTDOOR_SENSOR_H
#define OUTDOOR_SENSOR_H

#include <Arduino.h>
#include <DHT.h>

namespace ShadeWave {
namespace Sensor {

class OutdoorSensor {
private:
  DHT dht;
  float temperature;
  float humidity;
  bool ready;

public:
  OutdoorSensor(uint8_t pin);
  
  // Initialize the DHT22 sensor
  bool begin();
  
  // Read temperature and humidity from sensor
  // Returns true if read was successful
  bool read();
  
  // Getters for last readings
  float getTemperature() const { return temperature; }
  float getHumidity() const { return humidity; }
  
  // Check if sensor is connected and ready
  bool isReady() const { return ready; }
};

} // namespace Sensor
} // namespace ShadeWave

#endif // OUTDOOR_SENSOR_H
