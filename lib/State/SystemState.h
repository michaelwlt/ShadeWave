#ifndef SYSTEM_STATE_H
#define SYSTEM_STATE_H

#include <Arduino.h>

namespace ShadeWave {
namespace Controller {
  class VentController;
  class BlindsController;
}

namespace State {

class SystemState {
private:
  bool firstRun;

public:
  SystemState();
  
  bool isFirstRun() const { return firstRun; }
  void setFirstRunComplete() { firstRun = false; }
  
  void printSystemState(const Controller::VentController& vent, const Controller::BlindsController& blinds, bool forcePrint = false);
};

} // namespace State
} // namespace ShadeWave

#endif // SYSTEM_STATE_H

