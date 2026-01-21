#ifndef INDOOR_SENSOR_H
#define INDOOR_SENSOR_H

#include <Arduino.h>
#include <DFRobot_SHT20.h>

namespace ShadeWave {
namespace Sensor {

class IndoorSensor {
private:
  DFRobot_SHT20 sht20;
  float temperature;
  float humidity;
  bool ready;

public:
  IndoorSensor();
  
  // Initialize the SHT20 sensor
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

#endif // INDOOR_SENSOR_H
