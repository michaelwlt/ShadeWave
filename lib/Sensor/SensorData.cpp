#include "SensorData.h"
#include "Config.h"

namespace ShadeWave {
namespace Sensor {

const float SensorData::HOT_TEMP_THRESHOLD = ShadeWave::Config::HOT_TEMP_THRESHOLD;

SensorData::SensorData() 
  : roomOccupied(false),
    indoorTemp(25.0),
    outdoorTemp(20.0),
    indoorHumidity(50.0),
    outdoorHumidity(70.0),
    sunlightIntense(true),
    isHotInside(false),
    prevRoomOccupied(false),
    prevIndoorTemp(0.0),
    prevOutdoorTemp(0.0),
    prevIndoorHumidity(0.0),
    prevOutdoorHumidity(0.0),
    prevSunlightIntense(false),
    prevIsHotInside(false) {
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

void SensorData::updateDerivedValues() {
  isHotInside = (indoorTemp >= HOT_TEMP_THRESHOLD);
}

bool SensorData::hasChanged() const {
  return (roomOccupied != prevRoomOccupied) ||
         (indoorTemp != prevIndoorTemp) ||
         (outdoorTemp != prevOutdoorTemp) ||
         (indoorHumidity != prevIndoorHumidity) ||
         (outdoorHumidity != prevOutdoorHumidity) ||
         (sunlightIntense != prevSunlightIntense) ||
         (isHotInside != prevIsHotInside);
}

void SensorData::printReadings(bool forcePrint) {
  bool sensorChanged = hasChanged();
  
  if (sensorChanged || forcePrint) {
    Serial.println(F("\n--- Sensor Readings ---"));
    Serial.print(F("Room Occupied: ")); Serial.println(roomOccupied ? F("Yes") : F("No"));
    Serial.print(F("Indoor Temp: ")); Serial.print(indoorTemp); Serial.println(F("°C"));
    Serial.print(F("Outdoor Temp: ")); Serial.print(outdoorTemp); Serial.println(F("°C"));
    Serial.print(F("Indoor Humidity: ")); Serial.print(indoorHumidity); Serial.println(F("%"));
    Serial.print(F("Outdoor Humidity: ")); Serial.print(outdoorHumidity); Serial.println(F("%"));
    Serial.print(F("Sunlight Intense: ")); Serial.println(sunlightIntense ? F("Yes") : F("No"));
    Serial.print(F("Hot Inside: ")); Serial.println(isHotInside ? F("Yes (Summer)") : F("No (Winter)"));
    
    // Update previous values
    prevRoomOccupied = roomOccupied;
    prevIndoorTemp = indoorTemp;
    prevOutdoorTemp = outdoorTemp;
    prevIndoorHumidity = indoorHumidity;
    prevOutdoorHumidity = outdoorHumidity;
    prevSunlightIntense = sunlightIntense;
    prevIsHotInside = isHotInside;
  }
}

} // namespace Sensor
} // namespace ShadeWave

