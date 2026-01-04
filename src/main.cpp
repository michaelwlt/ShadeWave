#include <Arduino.h>

// Pin definitions for LEDs
// Vent control LEDs
#define VENT_OPEN_LED 2      // LED indicating vent is open
#define VENT_CLOSED_LED 3    // LED indicating vent is closed

// Blinds control LEDs
#define BLINDS_OPEN_LED 4    // LED indicating blinds are open
#define BLINDS_CLOSED_LED 5  // LED indicating blinds are closed
#define BLINDS_NEUTRAL_LED 6 // LED indicating blinds are neutral

// Dummy sensor values (can be changed for testing)
bool roomOccupied = false;           // Motion sensor: true = occupied, false = not occupied
float indoorTemp = 25.0;            // Indoor temperature in Celsius
float outdoorTemp = 20.0;           // Outdoor temperature in Celsius
float indoorHumidity = 50.0;        // Indoor humidity percentage
float outdoorHumidity = 70.0;       // Outdoor humidity percentage
bool sunlightIntense = true;        // Light sensor: true = intense, false = not intense
bool isHotInside = false;           // Derived: true if indoor temp is high (summer mode)

// State variables
bool ventOpen = false;
enum BlindsState {
  BLINDS_OPEN,
  BLINDS_CLOSED,
  BLINDS_NEUTRAL
};
BlindsState blindsState = BLINDS_NEUTRAL;

// Thresholds
const float HIGH_HUMIDITY_THRESHOLD = 75.0;  // Humidity percentage considered "high"
const float HOT_TEMP_THRESHOLD = 24.0;       // Temperature considered "hot" for summer mode

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(9600);
  Serial.println("Smart Environment Control System Starting...");
  
  // Initialize LED pins as outputs
  pinMode(VENT_OPEN_LED, OUTPUT);
  pinMode(VENT_CLOSED_LED, OUTPUT);
  pinMode(BLINDS_OPEN_LED, OUTPUT);
  pinMode(BLINDS_CLOSED_LED, OUTPUT);
  pinMode(BLINDS_NEUTRAL_LED, OUTPUT);
  
  // Turn off all LEDs initially
  digitalWrite(VENT_OPEN_LED, LOW);
  digitalWrite(VENT_CLOSED_LED, LOW);
  digitalWrite(BLINDS_OPEN_LED, LOW);
  digitalWrite(BLINDS_CLOSED_LED, LOW);
  digitalWrite(BLINDS_NEUTRAL_LED, LOW);
  
  Serial.println("System initialized. Starting control loop...");
  Serial.println("\n=== Serial Command Interface ===");
  Serial.println("Commands:");
  Serial.println("  set <variable> <value>  - Set sensor value");
  Serial.println("  get                    - Show all sensor values");
  Serial.println("  help                   - Show this help");
  Serial.println("Examples:");
  Serial.println("  set roomOccupied true");
  Serial.println("  set indoorTemp 26.5");
  Serial.println("  set outdoorTemp 22.0");
  Serial.println("  set indoorHumidity 60.0");
  Serial.println("  set outdoorHumidity 80.0");
  Serial.println("  set sunlightIntense false");
  Serial.println("================================");
  delay(1000);
}

void updateVentLEDs(bool open) {
  digitalWrite(VENT_OPEN_LED, open ? HIGH : LOW);
  digitalWrite(VENT_CLOSED_LED, open ? LOW : HIGH);
}

void updateBlindsLEDs(BlindsState state) {
  // Turn off all blinds LEDs first
  digitalWrite(BLINDS_OPEN_LED, LOW);
  digitalWrite(BLINDS_CLOSED_LED, LOW);
  digitalWrite(BLINDS_NEUTRAL_LED, LOW);
  
  // Turn on the appropriate LED
  switch(state) {
    case BLINDS_OPEN:
      digitalWrite(BLINDS_OPEN_LED, HIGH);
      break;
    case BLINDS_CLOSED:
      digitalWrite(BLINDS_CLOSED_LED, HIGH);
      break;
    case BLINDS_NEUTRAL:
      digitalWrite(BLINDS_NEUTRAL_LED, HIGH);
      break;
  }
}

void processSerialCommands() {
  // Check if serial data is available
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    command.toLowerCase();
    
    if (command.length() == 0) return;
    
    // Parse "set <variable> <value>" command
    if (command.startsWith("set ")) {
      command = command.substring(4); // Remove "set "
      int spaceIndex = command.indexOf(' ');
      if (spaceIndex > 0) {
        String varName = command.substring(0, spaceIndex);
        String valueStr = command.substring(spaceIndex + 1);
        valueStr.trim();
        
        // Parse boolean values
        if (varName == "roomoccupied") {
          if (valueStr == "true" || valueStr == "1") {
            roomOccupied = true;
            Serial.println("OK: roomOccupied set to true");
          } else if (valueStr == "false" || valueStr == "0") {
            roomOccupied = false;
            Serial.println("OK: roomOccupied set to false");
          } else {
            Serial.println("ERROR: Invalid value. Use 'true' or 'false'");
          }
        }
        else if (varName == "sunlightintense") {
          if (valueStr == "true" || valueStr == "1") {
            sunlightIntense = true;
            Serial.println("OK: sunlightIntense set to true");
          } else if (valueStr == "false" || valueStr == "0") {
            sunlightIntense = false;
            Serial.println("OK: sunlightIntense set to false");
          } else {
            Serial.println("ERROR: Invalid value. Use 'true' or 'false'");
          }
        }
        // Parse float values
        else if (varName == "indoortemp") {
          float val = valueStr.toFloat();
          if (val != 0.0 || valueStr == "0" || valueStr == "0.0") {
            indoorTemp = val;
            Serial.print("OK: indoorTemp set to ");
            Serial.println(indoorTemp);
            isHotInside = (indoorTemp >= HOT_TEMP_THRESHOLD);
          } else {
            Serial.println("ERROR: Invalid temperature value");
          }
        }
        else if (varName == "outdoortemp") {
          float val = valueStr.toFloat();
          if (val != 0.0 || valueStr == "0" || valueStr == "0.0") {
            outdoorTemp = val;
            Serial.print("OK: outdoorTemp set to ");
            Serial.println(outdoorTemp);
          } else {
            Serial.println("ERROR: Invalid temperature value");
          }
        }
        else if (varName == "indoorhumidity") {
          float val = valueStr.toFloat();
          if (val != 0.0 || valueStr == "0" || valueStr == "0.0") {
            indoorHumidity = val;
            Serial.print("OK: indoorHumidity set to ");
            Serial.println(indoorHumidity);
          } else {
            Serial.println("ERROR: Invalid humidity value");
          }
        }
        else if (varName == "outdoorhumidity") {
          float val = valueStr.toFloat();
          if (val != 0.0 || valueStr == "0" || valueStr == "0.0") {
            outdoorHumidity = val;
            Serial.print("OK: outdoorHumidity set to ");
            Serial.println(outdoorHumidity);
          } else {
            Serial.println("ERROR: Invalid humidity value");
          }
        }
        else {
          Serial.println("ERROR: Unknown variable name");
          Serial.println("Available: roomOccupied, indoorTemp, outdoorTemp, indoorHumidity, outdoorHumidity, sunlightIntense");
        }
      } else {
        Serial.println("ERROR: Invalid command format. Use: set <variable> <value>");
      }
    }
    // Parse "get" command
    else if (command == "get" || command == "status") {
      Serial.println("\n--- Current Sensor Values ---");
      Serial.print("roomOccupied: "); Serial.println(roomOccupied ? "true" : "false");
      Serial.print("indoorTemp: "); Serial.print(indoorTemp); Serial.println("°C");
      Serial.print("outdoorTemp: "); Serial.print(outdoorTemp); Serial.println("°C");
      Serial.print("indoorHumidity: "); Serial.print(indoorHumidity); Serial.println("%");
      Serial.print("outdoorHumidity: "); Serial.print(outdoorHumidity); Serial.println("%");
      Serial.print("sunlightIntense: "); Serial.println(sunlightIntense ? "true" : "false");
      Serial.print("isHotInside: "); Serial.println(isHotInside ? "true" : "false");
    }
    // Parse "help" command
    else if (command == "help") {
      Serial.println("\n=== Available Commands ===");
      Serial.println("set <variable> <value>  - Set sensor value");
      Serial.println("  Variables:");
      Serial.println("    roomOccupied        - true/false");
      Serial.println("    indoorTemp         - float (e.g., 25.5)");
      Serial.println("    outdoorTemp        - float (e.g., 20.0)");
      Serial.println("    indoorHumidity     - float (e.g., 50.0)");
      Serial.println("    outdoorHumidity    - float (e.g., 70.0)");
      Serial.println("    sunlightIntense    - true/false");
      Serial.println("get                    - Show all sensor values");
      Serial.println("help                   - Show this help");
      Serial.println("==========================");
    }
    else {
      Serial.println("ERROR: Unknown command. Type 'help' for available commands.");
    }
  }
}

void readSensorData() {
  // In a real system, this would read from actual sensors
  // Values can now be modified via serial commands at runtime
  // Only update derived values here
  isHotInside = (indoorTemp >= HOT_TEMP_THRESHOLD);
  
  Serial.println("\n--- Sensor Readings ---");
  Serial.print("Room Occupied: "); Serial.println(roomOccupied ? "Yes" : "No");
  Serial.print("Indoor Temp: "); Serial.print(indoorTemp); Serial.println("°C");
  Serial.print("Outdoor Temp: "); Serial.print(outdoorTemp); Serial.println("°C");
  Serial.print("Indoor Humidity: "); Serial.print(indoorHumidity); Serial.println("%");
  Serial.print("Outdoor Humidity: "); Serial.print(outdoorHumidity); Serial.println("%");
  Serial.print("Sunlight Intense: "); Serial.println(sunlightIntense ? "Yes" : "No");
  Serial.print("Hot Inside: "); Serial.println(isHotInside ? "Yes (Summer)" : "No (Winter)");
}

void controlVents() {
  Serial.println("\n--- Vent Control Logic ---");
  
  if (!roomOccupied) {
    // Energy saving mode: Close vents and blinds
    Serial.println("Room not occupied -> Energy Saving Mode");
    ventOpen = false;
    updateVentLEDs(false);
    Serial.println("Action: Close Vents");
    return;
  }
  
  // Room is occupied - compare temperatures
  Serial.println("Room occupied -> Checking temperature...");
  
  if (outdoorTemp < indoorTemp) {
    // Cooler outside
    Serial.println("Outdoor < Indoor (Cooler Outside)");
    
    if (outdoorHumidity > HIGH_HUMIDITY_THRESHOLD) {
      // High humidity outside
      Serial.println("Outdoor humidity is HIGH");
      ventOpen = false;
      updateVentLEDs(false);
      Serial.println("Action: Keep Vents Closed (high humidity)");
    } else {
      // Low humidity - natural cooling
      Serial.println("Outdoor humidity is acceptable");
      ventOpen = true;
      updateVentLEDs(true);
      Serial.println("Action: Open Vents - Natural Cooling");
    }
  } else {
    // Warmer outside
    Serial.println("Outdoor > Indoor (Warmer Outside)");
    ventOpen = false;
    updateVentLEDs(false);
    Serial.println("Action: Close Vents - Keep Cool Air In");
  }
}

void controlBlinds() {
  Serial.println("\n--- Blinds Control Logic ---");
  
  if (!roomOccupied) {
    // Energy saving mode: Close blinds
    Serial.println("Room not occupied -> Energy Saving Mode");
    blindsState = BLINDS_CLOSED;
    updateBlindsLEDs(BLINDS_CLOSED);
    Serial.println("Action: Close Blinds");
    return;
  }
  
  // Room is occupied - check sunlight
  if (sunlightIntense) {
    Serial.println("Sunlight is intense");
    
    if (isHotInside) {
      // Summer mode: Block heat
      Serial.println("Hot inside (Summer Mode)");
      blindsState = BLINDS_CLOSED;
      updateBlindsLEDs(BLINDS_CLOSED);
      Serial.println("Action: Close Blinds - Block Heat");
    } else {
      // Winter mode: Solar heating
      Serial.println("Not hot inside (Winter Mode)");
      blindsState = BLINDS_OPEN;
      updateBlindsLEDs(BLINDS_OPEN);
      Serial.println("Action: Open Blinds - Solar Heating");
    }
  } else {
    // Sunlight not intense
    Serial.println("Sunlight is not intense");
    blindsState = BLINDS_NEUTRAL;
    updateBlindsLEDs(BLINDS_NEUTRAL);
    Serial.println("Action: Keep Blinds Neutral");
  }
}

void loop() {
  // Process serial commands (non-blocking)
  processSerialCommands();
  
  // Main control loop following the flowchart
  
  // Step 1: Read Sensor Data
  readSensorData();
  
  // Step 2: Control Vents (based on occupancy and temperature)
  controlVents();
  
  // Step 3: Control Blinds (based on occupancy and sunlight)
  controlBlinds();
  
  // Step 4: Display current state
  Serial.println("\n--- Current System State ---");
  Serial.print("Vent: "); Serial.println(ventOpen ? "OPEN" : "CLOSED");
  Serial.print("Blinds: ");
  switch(blindsState) {
    case BLINDS_OPEN: Serial.println("OPEN"); break;
    case BLINDS_CLOSED: Serial.println("CLOSED"); break;
    case BLINDS_NEUTRAL: Serial.println("NEUTRAL"); break;
  }
  Serial.println("==============================");
  
  // Step 5: Wait / Loop (delay before next iteration)
  // Process commands during delay to be responsive
  unsigned long startTime = millis();
  while (millis() - startTime < 5000) {
    processSerialCommands();
    delay(100); // Small delay to avoid busy-waiting
  }
}
