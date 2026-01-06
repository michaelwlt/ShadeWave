#include <Arduino.h>
#include <string.h>
#include <stdlib.h>

// Pin definitions for LEDs
// Vent control LEDs
#define VENT_OPEN_LED 2      // LED indicating vent is open
#define VENT_CLOSED_LED 3    // LED indicating vent is closed

// Blinds control LEDs
#define BLINDS_OPEN_LED 4    // LED indicating blinds are open
#define BLINDS_CLOSED_LED 5  // LED indicating blinds are closed
#define BLINDS_NEUTRAL_LED 6 // LED indicating blinds are neutral

// Command buffer size
#define CMD_BUFFER_SIZE 64

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

// Previous state tracking (for change detection)
bool prevRoomOccupied = false;
float prevIndoorTemp = 0.0;
float prevOutdoorTemp = 0.0;
float prevIndoorHumidity = 0.0;
float prevOutdoorHumidity = 0.0;
bool prevSunlightIntense = false;
bool prevIsHotInside = false;
bool prevVentOpen = false;
BlindsState prevBlindsState = BLINDS_NEUTRAL;
bool firstRun = true; // Flag to print status on first run

// Thresholds
const float HIGH_HUMIDITY_THRESHOLD = 75.0;  // Humidity percentage considered "high"
const float HOT_TEMP_THRESHOLD = 24.0;       // Temperature considered "hot" for summer mode

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(9600);
  Serial.println(F("Smart Environment Control System Starting..."));
  
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
  
  // Test all LEDs to verify they work
  Serial.println(F("Testing all LEDs..."));
  
  Serial.println(F("Testing Pin 2 (Vent Open)..."));
  digitalWrite(VENT_OPEN_LED, HIGH);
  delay(2000);
  digitalWrite(VENT_OPEN_LED, LOW);
  delay(200);
  
  Serial.println(F("Testing Pin 3 (Vent Closed)..."));
  digitalWrite(VENT_CLOSED_LED, HIGH);
  delay(2000);
  digitalWrite(VENT_CLOSED_LED, LOW);
  delay(200);
  
  Serial.println(F("Testing Pin 4 (Blinds Open)..."));
  digitalWrite(BLINDS_OPEN_LED, HIGH);
  delay(2000);
  digitalWrite(BLINDS_OPEN_LED, LOW);
  delay(200);
  
  Serial.println(F("Testing Pin 5 (Blinds Closed)..."));
  digitalWrite(BLINDS_CLOSED_LED, HIGH);
  delay(2000);
  digitalWrite(BLINDS_CLOSED_LED, LOW);
  delay(200);
  
  Serial.println(F("Testing Pin 6 (Blinds Neutral)..."));
  digitalWrite(BLINDS_NEUTRAL_LED, HIGH);
  delay(2000);
  digitalWrite(BLINDS_NEUTRAL_LED, LOW);
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

// Helper function to convert string to lowercase in-place
void toLowerCase(char* str) {
  for (int i = 0; str[i]; i++) {
    if (str[i] >= 'A' && str[i] <= 'Z') {
      str[i] = str[i] - 'A' + 'a';
    }
  }
}

// Helper function to trim whitespace
void trim(char* str) {
  // Trim leading whitespace
  char* start = str;
  while (*start && (*start == ' ' || *start == '\t' || *start == '\r' || *start == '\n')) {
    start++;
  }
  
  // Move trimmed string to beginning
  if (start != str) {
    char* dst = str;
    while (*start) {
      *dst++ = *start++;
    }
    *dst = '\0';
  }
  
  // Trim trailing whitespace
  int len = strlen(str);
  while (len > 0 && (str[len-1] == ' ' || str[len-1] == '\t' || str[len-1] == '\r' || str[len-1] == '\n')) {
    str[len-1] = '\0';
    len--;
  }
}

// Helper function to compare strings (case-insensitive)
bool strEquals(const char* a, const char* b) {
  while (*a && *b) {
    char ca = *a;
    char cb = *b;
    // Convert to lowercase for comparison
    if (ca >= 'A' && ca <= 'Z') ca = ca - 'A' + 'a';
    if (cb >= 'A' && cb <= 'Z') cb = cb - 'A' + 'a';
    if (ca != cb) return false;
    a++;
    b++;
  }
  return (*a == '\0' && *b == '\0');
}

void processSerialCommands() {
  // Check if serial data is available
  if (Serial.available() > 0) {
    char command[CMD_BUFFER_SIZE];
    int index = 0;
    
    // Read until newline or buffer full
    while (index < CMD_BUFFER_SIZE - 1) {
      if (Serial.available() > 0) {
        char c = Serial.read();
        if (c == '\n' || c == '\r') break;
        command[index++] = c;
      }
    }
    command[index] = '\0';
    
    trim(command);
    toLowerCase(command);
    
    if (strlen(command) == 0) return;
    
    // Parse "set <variable> <value>" command
    if (strncmp(command, "set ", 4) == 0) {
      char* cmd = command + 4; // Skip "set "
      trim(cmd);
      
      // Find space between variable and value
      char* spacePos = strchr(cmd, ' ');
      if (spacePos != NULL) {
        *spacePos = '\0'; // Split string
        char* varName = cmd;
        char* valueStr = spacePos + 1;
        trim(valueStr);
        
        // Parse boolean values
        if (strEquals(varName, "roomoccupied")) {
          if (strEquals(valueStr, "true") || strEquals(valueStr, "1")) {
            roomOccupied = true;
            Serial.println(F("OK: roomOccupied set to true"));
          } else if (strEquals(valueStr, "false") || strEquals(valueStr, "0")) {
            roomOccupied = false;
            Serial.println(F("OK: roomOccupied set to false"));
          } else {
            Serial.println(F("ERROR: Invalid value. Use 'true' or 'false'"));
          }
        }
        else if (strEquals(varName, "sunlightintense")) {
          if (strEquals(valueStr, "true") || strEquals(valueStr, "1")) {
            sunlightIntense = true;
            Serial.println(F("OK: sunlightIntense set to true"));
          } else if (strEquals(valueStr, "false") || strEquals(valueStr, "0")) {
            sunlightIntense = false;
            Serial.println(F("OK: sunlightIntense set to false"));
          } else {
            Serial.println(F("ERROR: Invalid value. Use 'true' or 'false'"));
          }
        }
        // Parse float values
        else if (strEquals(varName, "indoortemp")) {
          float val = atof(valueStr);
          if (val != 0.0 || strEquals(valueStr, "0") || strEquals(valueStr, "0.0")) {
            indoorTemp = val;
            Serial.print(F("OK: indoorTemp set to "));
            Serial.println(indoorTemp);
            isHotInside = (indoorTemp >= HOT_TEMP_THRESHOLD);
          } else {
            Serial.println(F("ERROR: Invalid temperature value"));
          }
        }
        else if (strEquals(varName, "outdoortemp")) {
          float val = atof(valueStr);
          if (val != 0.0 || strEquals(valueStr, "0") || strEquals(valueStr, "0.0")) {
            outdoorTemp = val;
            Serial.print(F("OK: outdoorTemp set to "));
            Serial.println(outdoorTemp);
          } else {
            Serial.println(F("ERROR: Invalid temperature value"));
          }
        }
        else if (strEquals(varName, "indoorhumidity")) {
          float val = atof(valueStr);
          if (val != 0.0 || strEquals(valueStr, "0") || strEquals(valueStr, "0.0")) {
            indoorHumidity = val;
            Serial.print(F("OK: indoorHumidity set to "));
            Serial.println(indoorHumidity);
          } else {
            Serial.println(F("ERROR: Invalid humidity value"));
          }
        }
        else if (strEquals(varName, "outdoorhumidity")) {
          float val = atof(valueStr);
          if (val != 0.0 || strEquals(valueStr, "0") || strEquals(valueStr, "0.0")) {
            outdoorHumidity = val;
            Serial.print(F("OK: outdoorHumidity set to "));
            Serial.println(outdoorHumidity);
          } else {
            Serial.println(F("ERROR: Invalid humidity value"));
          }
        }
        else {
          Serial.println(F("ERROR: Unknown variable name"));
          Serial.println(F("Available: roomOccupied, indoorTemp, outdoorTemp, indoorHumidity, outdoorHumidity, sunlightIntense"));
        }
      } else {
        Serial.println(F("ERROR: Invalid command format. Use: set <variable> <value>"));
      }
    }
    // Parse "get" command
    else if (strEquals(command, "get") || strEquals(command, "status")) {
      Serial.println(F("\n--- Current Sensor Values ---"));
      Serial.print(F("roomOccupied: ")); Serial.println(roomOccupied ? F("true") : F("false"));
      Serial.print(F("indoorTemp: ")); Serial.print(indoorTemp); Serial.println(F("°C"));
      Serial.print(F("outdoorTemp: ")); Serial.print(outdoorTemp); Serial.println(F("°C"));
      Serial.print(F("indoorHumidity: ")); Serial.print(indoorHumidity); Serial.println(F("%"));
      Serial.print(F("outdoorHumidity: ")); Serial.print(outdoorHumidity); Serial.println(F("%"));
      Serial.print(F("sunlightIntense: ")); Serial.println(sunlightIntense ? F("true") : F("false"));
      Serial.print(F("isHotInside: ")); Serial.println(isHotInside ? F("true") : F("false"));
    }
    // Parse "help" command
    else if (strEquals(command, "help")) {
      Serial.println(F("\n=== Available Commands ==="));
      Serial.println(F("set <variable> <value>  - Set sensor value"));
      Serial.println(F("  Variables:"));
      Serial.println(F("    roomOccupied        - true/false"));
      Serial.println(F("    indoorTemp         - float (e.g., 25.5)"));
      Serial.println(F("    outdoorTemp        - float (e.g., 20.0)"));
      Serial.println(F("    indoorHumidity     - float (e.g., 50.0)"));
      Serial.println(F("    outdoorHumidity    - float (e.g., 70.0)"));
      Serial.println(F("    sunlightIntense    - true/false"));
      Serial.println(F("get                    - Show all sensor values"));
      Serial.println(F("help                   - Show this help"));
      Serial.println(F("=========================="));
    }
    else {
      Serial.println(F("ERROR: Unknown command. Type 'help' for available commands."));
    }
  }
}

void readSensorData() {
  // In a real system, this would read from actual sensors
  // Values can now be modified via serial commands at runtime
  // Only update derived values here
  isHotInside = (indoorTemp >= HOT_TEMP_THRESHOLD);
  
  // Check if any sensor value has changed
  bool sensorChanged = (roomOccupied != prevRoomOccupied) ||
                       (indoorTemp != prevIndoorTemp) ||
                       (outdoorTemp != prevOutdoorTemp) ||
                       (indoorHumidity != prevIndoorHumidity) ||
                       (outdoorHumidity != prevOutdoorHumidity) ||
                       (sunlightIntense != prevSunlightIntense) ||
                       (isHotInside != prevIsHotInside);
  
  // Only print if something changed or it's the first run
  if (sensorChanged || firstRun) {
    Serial.println(F("\n--- Sensor Readings ---"));
    Serial.print(F("Room Occupied: ")); Serial.println(roomOccupied ? F("Yes") : F("No"));
    Serial.print(F("Indoor Temp: ")); Serial.print(indoorTemp); Serial.println(F("°C"));
    Serial.print(F("Outdoor Temp: ")); Serial.print(outdoorTemp); Serial.println(F("°C"));
    Serial.print(F("Indoor Humidity: ")); Serial.print(indoorHumidity); Serial.println(F("%"));
    Serial.print(F("Outdoor Humidity: ")); Serial.print(outdoorHumidity); Serial.println(F("%"));
    Serial.print(F("Sunlight Intense: ")); Serial.println(sunlightIntense ? F("Yes") : F("No"));
    Serial.print(F("Hot Inside: ")); Serial.println(isHotInside ? F("Yes (Summer)") : F("No (Winter)"));
    
    // Update previous values
    prevRoomOccupied = roomOccupied;
    prevIndoorTemp = indoorTemp;
    prevOutdoorTemp = outdoorTemp;
    prevIndoorHumidity = indoorHumidity;
    prevOutdoorHumidity = outdoorHumidity;
    prevSunlightIntense = sunlightIntense;
    prevIsHotInside = isHotInside;
  }
}

void controlVents() {
  if (!roomOccupied) {
    // Energy saving mode: Close vents and blinds
    ventOpen = false;
    updateVentLEDs(false);
    
    // Only print if state changed
    if (ventOpen != prevVentOpen || firstRun) {
      Serial.println(F("\n--- Vent Control Logic ---"));
      Serial.println(F("Room not occupied -> Energy Saving Mode"));
      Serial.println(F("Action: Close Vents"));
    }
    return;
  }
  
  // Room is occupied - compare temperatures
  if (outdoorTemp < indoorTemp) {
    // Cooler outside
    if (outdoorHumidity > HIGH_HUMIDITY_THRESHOLD) {
      // High humidity outside
      ventOpen = false;
      updateVentLEDs(false);
    } else {
      // Low humidity - natural cooling
      ventOpen = true;
      updateVentLEDs(true);
    }
  } else {
    // Warmer outside
    ventOpen = false;
    updateVentLEDs(false);
  }
  
  // Only print if state changed
  if (ventOpen != prevVentOpen || firstRun) {
    Serial.println(F("\n--- Vent Control Logic ---"));
    Serial.println(F("Room occupied -> Checking temperature..."));
    
    if (outdoorTemp < indoorTemp) {
      Serial.println(F("Outdoor < Indoor (Cooler Outside)"));
      if (outdoorHumidity > HIGH_HUMIDITY_THRESHOLD) {
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

void controlBlinds() {
  if (!roomOccupied) {
    // Energy saving mode: Close blinds
    blindsState = BLINDS_CLOSED;
    updateBlindsLEDs(BLINDS_CLOSED);
    
    // Only print if state changed
    if (blindsState != prevBlindsState || firstRun) {
      Serial.println(F("\n--- Blinds Control Logic ---"));
      Serial.println(F("Room not occupied -> Energy Saving Mode"));
      Serial.println(F("Action: Close Blinds"));
    }
    return;
  }
  
  // Room is occupied - check sunlight
  if (sunlightIntense) {
    if (isHotInside) {
      // Summer mode: Block heat
      blindsState = BLINDS_CLOSED;
      updateBlindsLEDs(BLINDS_CLOSED);
    } else {
      // Winter mode: Solar heating
      blindsState = BLINDS_OPEN;
      updateBlindsLEDs(BLINDS_OPEN);
    }
  } else {
    // Sunlight not intense
    blindsState = BLINDS_NEUTRAL;
    updateBlindsLEDs(BLINDS_NEUTRAL);
  }
  
  // Only print if state changed
  if (blindsState != prevBlindsState || firstRun) {
    Serial.println(F("\n--- Blinds Control Logic ---"));
    if (sunlightIntense) {
      Serial.println(F("Sunlight is intense"));
      if (isHotInside) {
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
  
  // Step 4: Display current state (only if changed)
  bool stateChanged = (ventOpen != prevVentOpen) || (blindsState != prevBlindsState);
  if (stateChanged || firstRun) {
    Serial.println(F("\n--- Current System State ---"));
    Serial.print(F("Vent: ")); Serial.println(ventOpen ? F("OPEN") : F("CLOSED"));
    Serial.print(F("Blinds: "));
    switch(blindsState) {
      case BLINDS_OPEN: Serial.println(F("OPEN")); break;
      case BLINDS_CLOSED: Serial.println(F("CLOSED")); break;
      case BLINDS_NEUTRAL: Serial.println(F("NEUTRAL")); break;
    }
    Serial.println(F("=============================="));
  }
  
  // Update previous state variables
  prevVentOpen = ventOpen;
  prevBlindsState = blindsState;
  
  // Mark first run as complete
  if (firstRun) {
    firstRun = false;
  }
  
  // Step 5: Wait / Loop (delay before next iteration)
  // Process commands during delay to be responsive
  unsigned long startTime = millis();
  while (millis() - startTime < 5000) {
    processSerialCommands();
    delay(100); // Small delay to avoid busy-waiting
  }
}
