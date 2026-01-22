#ifndef CONFIG_H
#define CONFIG_H

namespace ShadeWave {
namespace Config {

// PIR motion sensor pin
const int PIR_SENSOR_PIN = 11;    // Digital pin for PIR motion sensor

// LDR light sensor pin (analog)
const int LDR_SENSOR_PIN = A0;    // Analog pin for LDR photo-resistor

// Indoor SHT20 sensor pins (Software I2C via SoftWire)
const int INDOOR_SDA_PIN = A1;    // Software I2C data pin
const int INDOOR_SCL_PIN = A2;    // Software I2C clock pin

// Servo configuration for vent
const int VENT_SERVO_PIN = 9;
const int VENT_SERVO_OPEN_ANGLE = 90;    // degrees when vent is open
const int VENT_SERVO_CLOSED_ANGLE = 0;   // degrees when vent is closed

// Servo configuration for blinds
const int BLINDS_SERVO_PIN = 10;
const int BLINDS_SERVO_2_PIN = 8;    // Second blinds servo (inverted)
const int BLINDS_SERVO_OPEN_ANGLE = 180;     // degrees when blinds are open
const int BLINDS_SERVO_NEUTRAL_ANGLE = 90;   // degrees when blinds are neutral
const int BLINDS_SERVO_CLOSED_ANGLE = 0;     // degrees when blinds are closed

// Thresholds
const float HOT_TEMP_THRESHOLD = 24.0;       // Temperature considered "hot" for summer mode
const int LDR_INTENSE_THRESHOLD = 300;       // Analog reading at or below this = intense sunlight (LDR: lower = brighter)

// Command buffer size
const int CMD_BUFFER_SIZE = 64;

// Timing configuration
const unsigned long MAIN_LOOP_INTERVAL_MS = 5000;  // Main loop runs every 5 seconds
const unsigned long PIR_WARMUP_MS = 20000;         // Motion sensor warm-up time (20 seconds)

} // namespace Config
} // namespace ShadeWave

#endif // CONFIG_H

