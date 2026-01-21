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
  Serial.println(F("[IndoorSensor] Initializing SHT20..."));
  
  // Set up SoftWire buffers
  softWire.setTxBuffer(swTxBuffer, sizeof(swTxBuffer));
  softWire.setRxBuffer(swRxBuffer, sizeof(swRxBuffer));
  softWire.setDelay_us(5);  // Standard I2C timing
  softWire.begin();
  
  delay(100);  // SHT20 power-up time
  
  // Send soft reset command
  softWire.beginTransmission(SHT20_ADDRESS);
  softWire.write(SHT20_SOFT_RESET);
  uint8_t resetResult = softWire.endTransmission();
  Serial.print(F("[IndoorSensor] Soft reset result: "));
  Serial.println(resetResult == 0 ? F("OK") : F("FAILED"));
  
  delay(15);  // Wait for reset to complete
  
  // Try a test read to verify sensor is connected
  uint16_t rawTemp;
  if (readSHT20Raw(SHT20_TEMP_NO_HOLD, rawTemp)) {
    temperature = rawToTemperature(rawTemp);
    Serial.print(F("[IndoorSensor] Initial temp: "));
    Serial.print(temperature);
    Serial.println(F(" C"));
    
    // Verify valid reading range
    if (temperature > -40.0 && temperature < 125.0) {
      uint16_t rawHumidity;
      if (readSHT20Raw(SHT20_HUMIDITY_NO_HOLD, rawHumidity)) {
        humidity = rawToHumidity(rawHumidity);
        Serial.print(F("[IndoorSensor] Initial humidity: "));
        Serial.print(humidity);
        Serial.println(F(" %"));
        ready = true;
        Serial.println(F("[IndoorSensor] Initialization SUCCESS"));
        return true;
      }
    }
  }
  
  ready = false;
  Serial.println(F("[IndoorSensor] Initialization FAILED"));
  return false;
}

bool IndoorSensor::readSHT20Raw(uint8_t cmd, uint16_t& rawValue) {
  // Send measurement command
  softWire.beginTransmission(SHT20_ADDRESS);
  softWire.write(cmd);
  uint8_t txResult = softWire.endTransmission();
  if (txResult != 0) {
    Serial.print(F("[IndoorSensor] I2C TX error: "));
    Serial.println(txResult);
    return false;  // NACK or error
  }
  
  // Wait for measurement to complete (no hold master mode)
  // Temperature max 85ms, humidity max 29ms at 14-bit resolution
  delay(cmd == SHT20_TEMP_NO_HOLD ? 85 : 29);
  
  // Request 3 bytes: MSB, LSB, Checksum
  uint8_t bytesReceived = softWire.requestFrom(SHT20_ADDRESS, (uint8_t)3);
  if (bytesReceived != 3) {
    Serial.print(F("[IndoorSensor] I2C RX error, got "));
    Serial.print(bytesReceived);
    Serial.println(F(" bytes (expected 3)"));
    return false;
  }
  
  uint8_t msb = softWire.read();
  uint8_t lsb = softWire.read();
  uint8_t checksum = softWire.read();  // Read but don't verify checksum
  (void)checksum;  // Suppress unused variable warning
  
  // Combine bytes (lower 2 bits of LSB are status bits, mask them)
  rawValue = ((uint16_t)msb << 8) | (lsb & 0xFC);
  
  Serial.print(F("[IndoorSensor] Raw "));
  Serial.print(cmd == SHT20_TEMP_NO_HOLD ? F("temp") : F("humidity"));
  Serial.print(F(": 0x"));
  Serial.println(rawValue, HEX);
  
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
    Serial.println(F("[IndoorSensor] read() called but sensor not ready"));
    return false;
  }
  
  uint16_t rawTemp, rawHumidity;
  
  if (!readSHT20Raw(SHT20_TEMP_NO_HOLD, rawTemp)) {
    Serial.println(F("[IndoorSensor] Failed to read temperature"));
    return false;
  }
  
  if (!readSHT20Raw(SHT20_HUMIDITY_NO_HOLD, rawHumidity)) {
    Serial.println(F("[IndoorSensor] Failed to read humidity"));
    return false;
  }
  
  float newTemp = rawToTemperature(rawTemp);
  float newHumidity = rawToHumidity(rawHumidity);
  
  // Validate readings are within sensor's operating range
  if (newTemp >= -40.0 && newTemp <= 125.0 &&
      newHumidity >= 0.0 && newHumidity <= 100.0) {
    temperature = newTemp;
    humidity = newHumidity;
    Serial.print(F("[IndoorSensor] Temp: "));
    Serial.print(temperature, 1);
    Serial.print(F(" C, Humidity: "));
    Serial.print(humidity, 1);
    Serial.println(F(" %"));
    return true;
  }
  
  Serial.println(F("[IndoorSensor] Reading out of valid range"));
  return false;
}

} // namespace Sensor
} // namespace ShadeWave
