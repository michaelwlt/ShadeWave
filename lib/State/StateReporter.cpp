#include "StateReporter.h"
#include "SensorData.h"
#include "VentController.h"
#include "BlindsController.h"
#include "Config.h"

namespace ShadeWave {
namespace State {

StateReporter::StateReporter() : needsReport(true) {
  // Initialize snapshot with default values
  lastReported.roomOccupied = false;
  lastReported.indoorTemp = 0.0f;
  lastReported.outdoorTemp = 0.0f;
  lastReported.indoorHumidity = 0.0f;
  lastReported.outdoorHumidity = 0.0f;
  lastReported.sunlightIntense = false;
  lastReported.isHotInside = false;
  lastReported.ventOpen = false;
  lastReported.blindsState = Controller::BlindsState::NEUTRAL;
}

bool StateReporter::hasChanged(const Sensor::SensorData& sensors,
                               const Controller::VentController& vent,
                               const Controller::BlindsController& blinds) const {
  return (sensors.getRoomOccupied() != lastReported.roomOccupied) ||
         (sensors.getIndoorTemp() != lastReported.indoorTemp) ||
         (sensors.getOutdoorTemp() != lastReported.outdoorTemp) ||
         (sensors.getIndoorHumidity() != lastReported.indoorHumidity) ||
         (sensors.getOutdoorHumidity() != lastReported.outdoorHumidity) ||
         (sensors.getSunlightIntense() != lastReported.sunlightIntense) ||
         (sensors.getIsHotInside() != lastReported.isHotInside) ||
         (vent.isOpen() != lastReported.ventOpen) ||
         (blinds.getState() != lastReported.blindsState);
}

void StateReporter::captureSnapshot(const Sensor::SensorData& sensors,
                                    const Controller::VentController& vent,
                                    const Controller::BlindsController& blinds) {
  lastReported.roomOccupied = sensors.getRoomOccupied();
  lastReported.indoorTemp = sensors.getIndoorTemp();
  lastReported.outdoorTemp = sensors.getOutdoorTemp();
  lastReported.indoorHumidity = sensors.getIndoorHumidity();
  lastReported.outdoorHumidity = sensors.getOutdoorHumidity();
  lastReported.sunlightIntense = sensors.getSunlightIntense();
  lastReported.isHotInside = sensors.getIsHotInside();
  lastReported.ventOpen = vent.isOpen();
  lastReported.blindsState = blinds.getState();
}

void StateReporter::reportIfChanged(const Sensor::SensorData& sensors,
                                    const Controller::VentController& vent,
                                    const Controller::BlindsController& blinds) {
  if (needsReport || hasChanged(sensors, vent, blinds)) {
    printVentLogic(sensors, vent);
    printBlindsLogic(sensors, blinds);
    printSystemSummary(vent, blinds);
    
    captureSnapshot(sensors, vent, blinds);
    needsReport = false;
  }
}

void StateReporter::printVentLogic(const Sensor::SensorData& sensors, const Controller::VentController& vent) {
  Serial.println(F("\n--- Vent Control Logic ---"));
  
  if (!sensors.getRoomOccupied()) {
    Serial.println(F("Room not occupied -> Energy Saving Mode"));
    Serial.println(F("Action: Close Vents"));
  } else {
    Serial.println(F("Room occupied -> Checking temperature..."));
    
    if (sensors.getOutdoorTemp() < sensors.getIndoorTemp()) {
      Serial.println(F("Outdoor < Indoor (Cooler Outside)"));
      if (sensors.getOutdoorHumidity() > sensors.getHumidityThreshold()) {
        Serial.println(F("Outdoor humidity is HIGH"));
        Serial.println(F("Action: Keep Vents Closed (high humidity)"));
      } else {
        Serial.println(F("Outdoor humidity is acceptable"));
        Serial.println(F("Action: Open Vents - Natural Cooling"));
      }
    } else {
      Serial.println(F("Outdoor > Indoor (Warmer Outside)"));
      Serial.println(F("Action: Close Vents - Keep Cool Air In"));
    }
  }
}

void StateReporter::printBlindsLogic(const Sensor::SensorData& sensors, const Controller::BlindsController& blinds) {
  Serial.println(F("\n--- Blinds Control Logic ---"));
  
  if (!sensors.getRoomOccupied()) {
    Serial.println(F("Room not occupied -> Energy Saving Mode"));
    Serial.println(F("Action: Close Blinds"));
  } else {
    if (sensors.getSunlightIntense()) {
      Serial.println(F("Sunlight is intense"));
      if (sensors.getIsHotInside()) {
        Serial.println(F("Hot inside"));
        Serial.println(F("Action: Close Blinds - Block Heat"));
      } else {
        Serial.println(F("Not hot inside"));
        Serial.println(F("Action: Open Blinds - Solar Heating"));
      }
    } else {
      Serial.println(F("Sunlight is not intense"));
      Serial.println(F("Action: Keep Blinds Neutral"));
    }
  }
}

void StateReporter::printSystemSummary(const Controller::VentController& vent, const Controller::BlindsController& blinds) {
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

} // namespace State
} // namespace ShadeWave
