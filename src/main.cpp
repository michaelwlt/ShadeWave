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
Sensor::OutdoorSensor outdoorSensor(Config::DHT_SENSOR_PIN);
Sensor::IndoorSensor indoorSensor(Config::DS18B20_SENSOR_PIN);
Sensor::MotionSensor motionSensor(Config::PIR_SENSOR_PIN);
Sensor::LightSensor lightSensor(Config::LDR_SENSOR_PIN);
Controller::VentController vent(Config::VENT_OPEN_LED, Config::VENT_CLOSED_LED, Config::VENT_SERVO_PIN);
Controller::BlindsController blinds(Config::BLINDS_OPEN_LED, Config::BLINDS_CLOSED_LED, Config::BLINDS_NEUTRAL_LED, Config::BLINDS_SERVO_PIN);
SerialCommand::SerialCommandProcessor cmdProcessor(sensors);
State::StateReporter stateReporter;

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(9600);
  Serial.println(F("Smart Environment Control System Starting..."));
  
  // Initialize controllers (sets up LED pins)
  vent.initialize();
  blinds.initialize();
  
  // Initialize outdoor sensor (DHT22)
  if (outdoorSensor.begin()) {
    Serial.println(F("DHT22 outdoor sensor initialized"));
  } else {
    Serial.println(F("DHT22 sensor not found!"));
  }
  
  // Initialize indoor sensor (DS18B20)
  if (indoorSensor.begin()) {
    Serial.println(F("DS18B20 indoor sensor initialized"));
  } else {
    Serial.println(F("DS18B20 sensor not found!"));
  }
  
    // Initialize motion sensor (PIR) - blocks during 30s warm-up
    motionSensor.begin();
  
  // Initialize light sensor (LDR)
  lightSensor.begin();
  Serial.println(F("LDR light sensor initialized"));
  
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
  
  Serial.println(F("Testing Pin 9 (Vent Servo)..."));
  Serial.println(F("  Sweeping: CLOSED -> OPEN -> CLOSED"));
  vent.testServo();
  
  Serial.println(F("Testing Pin 10 (Blinds Servo)..."));
  Serial.println(F("  Sweeping: CLOSED -> NEUTRAL -> OPEN -> NEUTRAL"));
  blinds.testServo();
  
  Serial.println(F("LED and Servo test complete. Starting system in 3 seconds..."));
  delay(3000);
  Serial.println(F("System initialized. Starting control loop..."));
  cmdProcessor.printHelp();
  delay(1000);
  
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
