#include "BlindsController.h"
#include "SensorData.h"
#include "Config.h"

namespace ShadeWave {
namespace Controller {

const float BlindsController::HOT_TEMP_THRESHOLD = ShadeWave::Config::HOT_TEMP_THRESHOLD;

BlindsController::BlindsController(int openPin, int closedPin, int neutralPin)
  : blindsState(BlindsState::NEUTRAL),
    prevBlindsState(BlindsState::NEUTRAL),
    openPin(openPin),
    closedPin(closedPin),
    neutralPin(neutralPin) {
}

void BlindsController::initialize() {
  pinMode(openPin, OUTPUT);
  pinMode(closedPin, OUTPUT);
  pinMode(neutralPin, OUTPUT);
  digitalWrite(openPin, LOW);
  digitalWrite(closedPin, LOW);
  digitalWrite(neutralPin, LOW);
}

void BlindsController::updateBlindsLEDs(BlindsState state) {
  // Turn off all blinds LEDs first
  digitalWrite(openPin, LOW);
  digitalWrite(closedPin, LOW);
  digitalWrite(neutralPin, LOW);
  
  // Turn on the appropriate LED
  switch(state) {
    case BlindsState::OPEN:
      digitalWrite(openPin, HIGH);
      break;
    case BlindsState::CLOSED:
      digitalWrite(closedPin, HIGH);
      break;
    case BlindsState::NEUTRAL:
      digitalWrite(neutralPin, HIGH);
      break;
  }
}

void BlindsController::update(const Sensor::SensorData& sensors) {
  if (!sensors.getRoomOccupied()) {
    // Energy saving mode: Close blinds
    blindsState = BlindsState::CLOSED;
    updateBlindsLEDs(BlindsState::CLOSED);
    return;
  }
  
  // Room is occupied - check sunlight
  if (sensors.getSunlightIntense()) {
    if (sensors.getIsHotInside()) {
      // Summer mode: Block heat
      blindsState = BlindsState::CLOSED;
      updateBlindsLEDs(BlindsState::CLOSED);
    } else {
      // Winter mode: Solar heating
      blindsState = BlindsState::OPEN;
      updateBlindsLEDs(BlindsState::OPEN);
    }
  } else {
    // Sunlight not intense
    blindsState = BlindsState::NEUTRAL;
    updateBlindsLEDs(BlindsState::NEUTRAL);
  }
}

void BlindsController::printStatus(const Sensor::SensorData& sensors, bool forcePrint) {
  bool stateChanged = hasChanged();
  
  if (stateChanged || forcePrint) {
    Serial.println(F("\n--- Blinds Control Logic ---"));
    
    if (!sensors.getRoomOccupied()) {
      Serial.println(F("Room not occupied -> Energy Saving Mode"));
      Serial.println(F("Action: Close Blinds"));
    } else {
      if (sensors.getSunlightIntense()) {
        Serial.println(F("Sunlight is intense"));
        if (sensors.getIsHotInside()) {
          Serial.println(F("Hot inside (Summer Mode)"));
          Serial.println(F("Action: Close Blinds - Block Heat"));
        } else {
          Serial.println(F("Not hot inside (Winter Mode)"));
          Serial.println(F("Action: Open Blinds - Solar Heating"));
        }
      } else {
        Serial.println(F("Sunlight is not intense"));
        Serial.println(F("Action: Keep Blinds Neutral"));
      }
    }
    
    prevBlindsState = blindsState;
  }
}

} // namespace Controller
} // namespace ShadeWave

