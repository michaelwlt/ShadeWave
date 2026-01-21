#ifndef INDOOR_SENSOR_H
#define INDOOR_SENSOR_H

#include <Arduino.h>
#include <SoftWire.h>

namespace ShadeWave {
namespace Sensor {

// SHT20 I2C address and commands
const uint8_t SHT20_ADDRESS = 0x40;
const uint8_t SHT20_TEMP_NO_HOLD = 0xF3;
const uint8_t SHT20_HUMIDITY_NO_HOLD = 0xF5;
const uint8_t SHT20_SOFT_RESET = 0xFE;

class IndoorSensor {
private:
  SoftWire softWire;
  char swTxBuffer[16];  // SoftWire requires user-provided TX buffer
  char swRxBuffer[16];  // SoftWire requires user-provided RX buffer
  float temperature;
  float humidity;
  bool ready;
  
  // SHT20 protocol helper - reads raw 16-bit value from sensor
  bool readSHT20Raw(uint8_t cmd, uint16_t& rawValue);
  
  // Convert raw values to actual readings
  float rawToTemperature(uint16_t raw);
  float rawToHumidity(uint16_t raw);

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
