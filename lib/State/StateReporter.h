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
    bool ventOpen;
    Controller::BlindsState blindsState;
  };
  
  Snapshot lastReported;
  bool needsReport;
  
  bool hasChanged(const Controller::VentController& vent,
                  const Controller::BlindsController& blinds) const;
  
  void captureSnapshot(const Controller::VentController& vent,
                       const Controller::BlindsController& blinds);
  
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
