#include "SensorData.h"
#include "Config.h"

namespace ShadeWave {
namespace Sensor {

SensorData::SensorData() 
  : roomOccupied(false),
    indoorTemp(25.0),
    outdoorTemp(20.0),
    indoorHumidity(50.0),
    outdoorHumidity(70.0),
    sunlightIntense(true),
    isHotInside(false),
    desiredTempThreshold(ShadeWave::Config::HOT_TEMP_THRESHOLD),
    humidityThreshold(ShadeWave::Config::HIGH_HUMIDITY_THRESHOLD) {
  updateDerivedValues();
}

void SensorData::setRoomOccupied(bool value) {
  roomOccupied = value;
}

void SensorData::setIndoorTemp(float value) {
  indoorTemp = value;
  updateDerivedValues();
}

void SensorData::setOutdoorTemp(float value) {
  outdoorTemp = value;
}

void SensorData::setIndoorHumidity(float value) {
  indoorHumidity = value;
}

void SensorData::setOutdoorHumidity(float value) {
  outdoorHumidity = value;
}

void SensorData::setSunlightIntense(bool value) {
  sunlightIntense = value;
}

void SensorData::setDesiredTempThreshold(float value) {
  desiredTempThreshold = value;
  updateDerivedValues(); // Recalculate isHotInside with new threshold
}

void SensorData::setHumidityThreshold(float value) {
  humidityThreshold = value;
}

void SensorData::updateDerivedValues() {
  isHotInside = (indoorTemp >= desiredTempThreshold);
}

} // namespace Sensor
} // namespace ShadeWave
