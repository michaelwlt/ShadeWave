#ifndef STATE_REPORTER_H
#define STATE_REPORTER_H

#include <Arduino.h>
#include "BlindsController.h"

namespace ShadeWave {
namespace Sensor {
  class SensorData;
}
namespace Controller {
  class VentController;
  class BlindsController;
}

namespace State {

class StateReporter {
private:
  struct Snapshot {
    bool roomOccupied;
    float indoorTemp;
    float outdoorTemp;
    float indoorHumidity;
    float outdoorHumidity;
    bool sunlightIntense;
    bool isHotInside;
    bool ventOpen;
    Controller::BlindsState blindsState;
  };
  
  Snapshot lastReported;
  bool needsReport;
  
  bool hasChanged(const Sensor::SensorData& sensors,
                  const Controller::VentController& vent,
                  const Controller::BlindsController& blinds) const;
  
  void captureSnapshot(const Sensor::SensorData& sensors,
                       const Controller::VentController& vent,
                       const Controller::BlindsController& blinds);
  
  void printSensorReadings(const Sensor::SensorData& sensors);
  void printVentLogic(const Sensor::SensorData& sensors, const Controller::VentController& vent);
  void printBlindsLogic(const Sensor::SensorData& sensors, const Controller::BlindsController& blinds);
  void printSystemSummary(const Controller::VentController& vent, const Controller::BlindsController& blinds);

public:
  StateReporter();
  
  void reportIfChanged(const Sensor::SensorData& sensors,
                       const Controller::VentController& vent,
                       const Controller::BlindsController& blinds);
};

} // namespace State
} // namespace ShadeWave

#endif // STATE_REPORTER_H
