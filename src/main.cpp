#include <Arduino.h>
#include <Config.h>
#include <SensorData.h>
#include <OutdoorSensor.h>
#include <IndoorSensor.h>
#include <MotionSensor.h>
#include <LightSensor.h>
#include <VentController.h>
#include <BlindsController.h>
#include <SerialCommandProcessor.h>
#include <StateReporter.h>

using namespace ShadeWave;

// Global instances (required by Arduino framework)
Sensor::SensorData sensors;
Sensor::OutdoorSensor outdoorSensor;
Sensor::IndoorSensor indoorSensor;
Sensor::MotionSensor motionSensor(Config::PIR_SENSOR_PIN);
Sensor::LightSensor lightSensor(Config::LDR_SENSOR_PIN);
Controller::VentController vent(Config::VENT_SERVO_PIN);
Controller::BlindsController blinds(Config::BLINDS_SERVO_PIN);
SerialCommand::SerialCommandProcessor cmdProcessor(sensors);
State::StateReporter stateReporter;

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  
  // Run setup wizard
  cmdProcessor.runSetupWizard();
  delay(2000);

  // Initialize hardware
  Serial.println(F("Initializing hardware..."));
  
  // Initialize controllers (sets up servo pins and tests them)
  Serial.print(F("  Vent controller... "));
  vent.initialize();
  Serial.println(F("OK"));
  
  Serial.print(F("  Blinds controller... "));
  blinds.initialize();
  Serial.println(F("OK"));
  
  // Initialize outdoor sensor (SHT20)
  Serial.print(F("  Outdoor sensor... "));
  if (outdoorSensor.begin()) {
    Serial.println(F("OK"));
  } else {
    Serial.println(F("NOT FOUND"));
  }
  
  // Initialize indoor sensor (SHT20 via Software I2C)
  Serial.print(F("  Indoor sensor... "));
  if (indoorSensor.begin()) {
    Serial.println(F("OK"));
  } else {
    Serial.println(F("NOT FOUND"));
  }
  
  // Initialize motion sensor
  motionSensor.begin();
  
  // Initialize light sensor
  Serial.print(F("  Light sensor... "));
  lightSensor.begin();
  Serial.println(F("OK"));
  
  Serial.println(F("\nHardware initialization complete."));
  Serial.println(F("Starting control loop..."));
  
  // Initial state update and report
  vent.update(sensors);
  blinds.update(sensors);
  stateReporter.reportIfChanged(sensors, vent, blinds);
}

void loop() {
  // Process serial commands continuously (non-blocking)
  cmdProcessor.process();
  
  // Non-blocking timing: only run main control loop at configured interval
  static unsigned long lastMainLoopTime = 0;
  
  // Check if enough time has passed since last main loop execution
  unsigned long currentTime = millis();
  if (currentTime - lastMainLoopTime < Config::MAIN_LOOP_INTERVAL_MS) {
    // Not enough time has passed, just process commands and return
    return;
  }
  
  // Update last execution time
  lastMainLoopTime = currentTime;
  
  // Main control loop
  
  // Read outdoor sensor and update sensor data
  if (outdoorSensor.read()) {
    sensors.setOutdoorTemp(outdoorSensor.getTemperature());
    sensors.setOutdoorHumidity(outdoorSensor.getHumidity());
  }
  
  // Read indoor sensor and update sensor data
  if (indoorSensor.read()) {
    sensors.setIndoorTemp(indoorSensor.getTemperature());
    sensors.setIndoorHumidity(indoorSensor.getHumidity());
  }
  
  // Read motion sensor and update sensor data
  if (motionSensor.read()) {
    sensors.setRoomOccupied(motionSensor.isMotionDetected());
  }
  
  // Read light sensor and update sensor data
  if (lightSensor.read()) {
    sensors.setSunlightIntense(lightSensor.isLightIntense());
  }
  
  // Update controllers based on current sensor state
  vent.update(sensors);
  blinds.update(sensors);
  
  // Report if anything changed
  stateReporter.reportIfChanged(sensors, vent, blinds);
}
