#ifndef CONFIG_H
#define CONFIG_H

namespace ShadeWave {
namespace Config {

// Pin definitions for LEDs
// Vent control LEDs
const int VENT_OPEN_LED = 2;      // LED indicating vent is open
const int VENT_CLOSED_LED = 3;    // LED indicating vent is closed

// Blinds control LEDs
const int BLINDS_OPEN_LED = 4;    // LED indicating blinds are open
const int BLINDS_CLOSED_LED = 5;  // LED indicating blinds are closed
const int BLINDS_NEUTRAL_LED = 6; // LED indicating blinds are neutral

// DHT22 sensor pin (outdoor)
const int DHT_SENSOR_PIN = 7;     // Digital pin for DHT22 data

// DS18B20 indoor temperature sensor pin
const int DS18B20_SENSOR_PIN = 8; // Digital pin for DS18B20 data

// PIR motion sensor pin
const int PIR_SENSOR_PIN = 11;    // Digital pin for PIR motion sensor

// Command buffer size
const int CMD_BUFFER_SIZE = 64;

// Servo configuration for vent
const int VENT_SERVO_PIN = 9;
const int VENT_SERVO_OPEN_ANGLE = 90;    // degrees when vent is open
const int VENT_SERVO_CLOSED_ANGLE = 0;   // degrees when vent is closed

// Servo configuration for blinds
const int BLINDS_SERVO_PIN = 10;
const int BLINDS_SERVO_OPEN_ANGLE = 180;     // degrees when blinds are open
const int BLINDS_SERVO_NEUTRAL_ANGLE = 90;   // degrees when blinds are neutral
const int BLINDS_SERVO_CLOSED_ANGLE = 0;     // degrees when blinds are closed

// Thresholds
const float HIGH_HUMIDITY_THRESHOLD = 75.0;  // Humidity percentage considered "high"
const float HOT_TEMP_THRESHOLD = 24.0;       // Temperature considered "hot" for summer mode

// Timing configuration
const unsigned long MAIN_LOOP_INTERVAL_MS = 5000;  // Main loop runs every 5 seconds
const unsigned long PIR_WARMUP_MS = 30000;         // PIR sensor warm-up time (30 seconds)

} // namespace Config
} // namespace ShadeWave

#endif // CONFIG_H

