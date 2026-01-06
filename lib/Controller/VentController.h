#ifndef VENT_CONTROLLER_H
#define VENT_CONTROLLER_H

#include <Arduino.h>

namespace ShadeWave {
namespace Sensor {
  class SensorData;
}

namespace Controller {

class VentController {
private:
  bool ventOpen;
  bool prevVentOpen;
  int openPin;
  int closedPin;
  static const float HIGH_HUMIDITY_THRESHOLD;
  
  void updateVentLEDs(bool open);

public:
  VentController(int openPin, int closedPin);
  
  void initialize();
  void update(const Sensor::SensorData& sensors);
  
  bool isOpen() const { return ventOpen; }
  bool hasChanged() const { return ventOpen != prevVentOpen; }
  
  void printStatus(const Sensor::SensorData& sensors, bool forcePrint = false);
};

} // namespace Controller
} // namespace ShadeWave

#endif // VENT_CONTROLLER_H

