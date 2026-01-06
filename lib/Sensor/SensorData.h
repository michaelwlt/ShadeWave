#ifndef SENSOR_DATA_H
#define SENSOR_DATA_H

#include <Arduino.h>

namespace ShadeWave {
namespace Sensor {

class SensorData {
private:
  // Sensor values
  bool roomOccupied;
  float indoorTemp;
  float outdoorTemp;
  float indoorHumidity;
  float outdoorHumidity;
  bool sunlightIntense;
  bool isHotInside;  // Derived value
  
  // Previous values for change detection
  bool prevRoomOccupied;
  float prevIndoorTemp;
  float prevOutdoorTemp;
  float prevIndoorHumidity;
  float prevOutdoorHumidity;
  bool prevSunlightIntense;
  bool prevIsHotInside;
  
  // Thresholds
  static const float HOT_TEMP_THRESHOLD;

public:
  SensorData();
  
  // Getters
  bool getRoomOccupied() const { return roomOccupied; }
  float getIndoorTemp() const { return indoorTemp; }
  float getOutdoorTemp() const { return outdoorTemp; }
  float getIndoorHumidity() const { return indoorHumidity; }
  float getOutdoorHumidity() const { return outdoorHumidity; }
  bool getSunlightIntense() const { return sunlightIntense; }
  bool getIsHotInside() const { return isHotInside; }
  
  // Setters
  void setRoomOccupied(bool value);
  void setIndoorTemp(float value);
  void setOutdoorTemp(float value);
  void setIndoorHumidity(float value);
  void setOutdoorHumidity(float value);
  void setSunlightIntense(bool value);
  
  // Derived value calculation
  void updateDerivedValues();
  
  // Change detection
  bool hasChanged() const;
  
  // Serial output
  void printReadings(bool forcePrint = false);
};

} // namespace Sensor
} // namespace ShadeWave

#endif // SENSOR_DATA_H

