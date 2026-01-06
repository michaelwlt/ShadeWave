#include "SystemState.h"
#include "VentController.h"
#include "BlindsController.h"

namespace ShadeWave {
namespace State {

SystemState::SystemState() : firstRun(true) {
}

void SystemState::printSystemState(const Controller::VentController& vent, const Controller::BlindsController& blinds, bool forcePrint) {
  bool stateChanged = vent.hasChanged() || blinds.hasChanged();
  
  if (stateChanged || forcePrint) {
    Serial.println(F("\n--- Current System State ---"));
    Serial.print(F("Vent: ")); Serial.println(vent.isOpen() ? F("OPEN") : F("CLOSED"));
    Serial.print(F("Blinds: "));
    switch(blinds.getState()) {
      case Controller::BlindsState::OPEN: Serial.println(F("OPEN")); break;
      case Controller::BlindsState::CLOSED: Serial.println(F("CLOSED")); break;
      case Controller::BlindsState::NEUTRAL: Serial.println(F("NEUTRAL")); break;
    }
    Serial.println(F("=============================="));
  }
}

} // namespace State
} // namespace ShadeWave

