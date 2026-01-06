#include <Arduino.h>
#include <Config.h>
#include <SensorData.h>
#include <VentController.h>
#include <BlindsController.h>
#include <SerialCommandProcessor.h>
#include <SystemState.h>

using namespace ShadeWave;

// Global instances (required by Arduino framework)
Sensor::SensorData sensors;
Controller::VentController vent(Config::VENT_OPEN_LED, Config::VENT_CLOSED_LED);
Controller::BlindsController blinds(Config::BLINDS_OPEN_LED, Config::BLINDS_CLOSED_LED, Config::BLINDS_NEUTRAL_LED);
SerialCommand::SerialCommandProcessor cmdProcessor(sensors);
State::SystemState systemState;

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(9600);
  Serial.println(F("Smart Environment Control System Starting..."));
  
  // Initialize controllers (sets up LED pins)
  vent.initialize();
  blinds.initialize();
  
  // Test all LEDs to verify they work
  Serial.println(F("Testing all LEDs..."));
  
  Serial.println(F("Testing Pin 2 (Vent Open)..."));
  digitalWrite(Config::VENT_OPEN_LED, HIGH);
  delay(2000);
  digitalWrite(Config::VENT_OPEN_LED, LOW);
  delay(200);
  
  Serial.println(F("Testing Pin 3 (Vent Closed)..."));
  digitalWrite(Config::VENT_CLOSED_LED, HIGH);
  delay(2000);
  digitalWrite(Config::VENT_CLOSED_LED, LOW);
  delay(200);
  
  Serial.println(F("Testing Pin 4 (Blinds Open)..."));
  digitalWrite(Config::BLINDS_OPEN_LED, HIGH);
  delay(2000);
  digitalWrite(Config::BLINDS_OPEN_LED, LOW);
  delay(200);
  
  Serial.println(F("Testing Pin 5 (Blinds Closed)..."));
  digitalWrite(Config::BLINDS_CLOSED_LED, HIGH);
  delay(2000);
  digitalWrite(Config::BLINDS_CLOSED_LED, LOW);
  delay(200);
  
  Serial.println(F("Testing Pin 6 (Blinds Neutral)..."));
  digitalWrite(Config::BLINDS_NEUTRAL_LED, HIGH);
  delay(2000);
  digitalWrite(Config::BLINDS_NEUTRAL_LED, LOW);
  delay(200);
  
  Serial.println(F("LED test complete. Starting system in 3 seconds..."));
  delay(3000);
  Serial.println(F("System initialized. Starting control loop..."));
  Serial.println(F("\n=== Serial Command Interface ==="));
  Serial.println(F("Commands:"));
  Serial.println(F("  set <variable> <value>  - Set sensor value"));
  Serial.println(F("  get                    - Show all sensor values"));
  Serial.println(F("  help                   - Show this help"));
  Serial.println(F("Examples:"));
  Serial.println(F("  set roomOccupied true"));
  Serial.println(F("  set indoorTemp 26.5"));
  Serial.println(F("  set outdoorTemp 22.0"));
  Serial.println(F("  set indoorHumidity 60.0"));
  Serial.println(F("  set outdoorHumidity 80.0"));
  Serial.println(F("  set sunlightIntense false"));
  Serial.println(F("================================"));
  delay(1000);
}

void loop() {
  // Process serial commands (non-blocking)
  cmdProcessor.process();
  
  // Main control loop following the flowchart
  
  // Step 1: Read Sensor Data
  sensors.printReadings(systemState.isFirstRun());
  
  // Step 2: Control Vents (based on occupancy and temperature)
  vent.update(sensors);
  vent.printStatus(sensors, systemState.isFirstRun());
  
  // Step 3: Control Blinds (based on occupancy and sunlight)
  blinds.update(sensors);
  blinds.printStatus(sensors, systemState.isFirstRun());
  
  // Step 4: Display current system state (only if changed)
  systemState.printSystemState(vent, blinds, systemState.isFirstRun());
  
  // Mark first run as complete
  if (systemState.isFirstRun()) {
    systemState.setFirstRunComplete();
  }
  
  // Step 5: Wait / Loop (delay before next iteration)
  // Process commands during delay to be responsive
  unsigned long startTime = millis();
  while (millis() - startTime < 5000) {
    cmdProcessor.process();
    delay(100); // Small delay to avoid busy-waiting
  }
}
