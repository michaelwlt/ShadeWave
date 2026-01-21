#include "IndoorSensor.h"
#include "../Config/Config.h"

namespace ShadeWave {
namespace Sensor {

IndoorSensor::IndoorSensor()
  : softWire(Config::INDOOR_SDA_PIN, Config::INDOOR_SCL_PIN),
    temperature(0.0),
    humidity(0.0),
    ready(false) {
}

bool IndoorSensor::begin() {
  // Set up SoftWire buffers
  softWire.setTxBuffer(swTxBuffer, sizeof(swTxBuffer));
  softWire.setRxBuffer(swRxBuffer, sizeof(swRxBuffer));
  softWire.setDelay_us(5);  // Standard I2C timing
  softWire.begin();
  
  delay(100);  // SHT20 power-up time
  
  // Send soft reset command
  softWire.beginTransmission(SHT20_ADDRESS);
  softWire.write(SHT20_SOFT_RESET);
  softWire.endTransmission();
  
  delay(15);  // Wait for reset to complete
  
  // Try a test read to verify sensor is connected
  uint16_t rawTemp;
  if (readSHT20Raw(SHT20_TEMP_NO_HOLD, rawTemp)) {
    temperature = rawToTemperature(rawTemp);
    
    // Verify valid reading range
    if (temperature > -40.0 && temperature < 125.0) {
      uint16_t rawHumidity;
      if (readSHT20Raw(SHT20_HUMIDITY_NO_HOLD, rawHumidity)) {
        humidity = rawToHumidity(rawHumidity);
        ready = true;
        return true;
      }
    }
  }
  
  ready = false;
  return false;
}

bool IndoorSensor::readSHT20Raw(uint8_t cmd, uint16_t& rawValue) {
  // Send measurement command
  softWire.beginTransmission(SHT20_ADDRESS);
  softWire.write(cmd);
  if (softWire.endTransmission() != 0) {
    return false;  // NACK or error
  }
  
  // Wait for measurement to complete (no hold master mode)
  // Temperature max 85ms, humidity max 29ms at 14-bit resolution
  delay(cmd == SHT20_TEMP_NO_HOLD ? 85 : 29);
  
  // Request 3 bytes: MSB, LSB, Checksum
  uint8_t bytesReceived = softWire.requestFrom(SHT20_ADDRESS, (uint8_t)3);
  if (bytesReceived != 3) {
    return false;
  }
  
  uint8_t msb = softWire.read();
  uint8_t lsb = softWire.read();
  uint8_t checksum = softWire.read();  // Read but don't verify checksum
  (void)checksum;  // Suppress unused variable warning
  
  // Combine bytes (lower 2 bits of LSB are status bits, mask them)
  rawValue = ((uint16_t)msb << 8) | (lsb & 0xFC);
  
  return true;
}

float IndoorSensor::rawToTemperature(uint16_t raw) {
  // Formula from SHT20 datasheet: T = -46.85 + 175.72 * (raw / 65536)
  return -46.85 + 175.72 * ((float)raw / 65536.0);
}

float IndoorSensor::rawToHumidity(uint16_t raw) {
  // Formula from SHT20 datasheet: RH = -6 + 125 * (raw / 65536)
  float humidity = -6.0 + 125.0 * ((float)raw / 65536.0);
  
  // Clamp to valid range
  if (humidity < 0.0) humidity = 0.0;
  if (humidity > 100.0) humidity = 100.0;
  
  return humidity;
}

bool IndoorSensor::read() {
  if (!ready) {
    return false;
  }
  
  uint16_t rawTemp, rawHumidity;
  
  if (!readSHT20Raw(SHT20_TEMP_NO_HOLD, rawTemp)) {
    return false;
  }
  
  if (!readSHT20Raw(SHT20_HUMIDITY_NO_HOLD, rawHumidity)) {
    return false;
  }
  
  float newTemp = rawToTemperature(rawTemp);
  float newHumidity = rawToHumidity(rawHumidity);
  
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
