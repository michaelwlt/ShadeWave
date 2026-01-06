#ifndef BLINDS_CONTROLLER_H
#define BLINDS_CONTROLLER_H

#include <Arduino.h>

namespace ShadeWave {
namespace Sensor {
  class SensorData;
}

namespace Controller {

enum class BlindsState {
  OPEN,
  CLOSED,
  NEUTRAL
};

class BlindsController {
private:
  BlindsState blindsState;
  BlindsState prevBlindsState;
  int openPin;
  int closedPin;
  int neutralPin;
  static const float HOT_TEMP_THRESHOLD;
  
  void updateBlindsLEDs(BlindsState state);

public:
  BlindsController(int openPin, int closedPin, int neutralPin);
  
  void initialize();
  void update(const Sensor::SensorData& sensors);
  
  BlindsState getState() const { return blindsState; }
  bool hasChanged() const { return blindsState != prevBlindsState; }
  
  void printStatus(const Sensor::SensorData& sensors, bool forcePrint = false);
};

} // namespace Controller
} // namespace ShadeWave

#endif // BLINDS_CONTROLLER_H

