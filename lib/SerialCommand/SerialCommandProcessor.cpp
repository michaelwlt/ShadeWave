#include "SerialCommandProcessor.h"
#include "SensorData.h"
#include "Config.h"
#include <string.h>
#include <stdlib.h>

namespace ShadeWave {
namespace SerialCommand {

const int SerialCommandProcessor::CMD_BUFFER_SIZE = ShadeWave::Config::CMD_BUFFER_SIZE;

SerialCommandProcessor::SerialCommandProcessor(Sensor::SensorData& sensors)
  : sensors(sensors) {
}

void SerialCommandProcessor::runSetupWizard() {
  // Display welcome banner
  Serial.println(F("\n========================================"));
  Serial.println(F("   SHADEWAVE - Smart Environment Control"));
  Serial.println(F("========================================"));
  Serial.println();
  Serial.println(F("Welcome! Let's configure your system."));
  Serial.println();
  Serial.println(F("The temperature threshold determines when"));
  Serial.println(F("the system considers the room \"too hot\""));
  Serial.println(F("and activates cooling (opens vents)."));
  Serial.println();
  Serial.println(F("Enter your preferred temperature"));
  Serial.println(F("in degrees Celsius (e.g. 23.0):"));
  
  char input[CMD_BUFFER_SIZE];
  
  while (true) {
    Serial.print(F("> "));
    
    // Wait for input (blocking)
    while (Serial.available() == 0) {
      // Wait
    }
    
    // Read input line using helper
    readSerialLine(input, CMD_BUFFER_SIZE);
    trim(input);
    
    if (strlen(input) == 0) {
      Serial.println(F("Please enter a value."));
      continue;
    }
    
    // Validate and set temperature using helper
    if (setTemperatureFromInput(input)) {
      Serial.println(F("\nSetup complete!"));
      Serial.println(F("Type 'help' to see available commands."));
      Serial.println();
      break;
    }
    // If validation failed, loop continues for retry
  }
}

void SerialCommandProcessor::toLowerCase(char* str) {
  for (int i = 0; str[i]; i++) {
    if (str[i] >= 'A' && str[i] <= 'Z') {
      str[i] = str[i] - 'A' + 'a';
    }
  }
}

void SerialCommandProcessor::trim(char* str) {
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

bool SerialCommandProcessor::strEquals(const char* a, const char* b) {
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

int SerialCommandProcessor::readSerialLine(char* buffer, int maxLen) {
  int index = 0;
  while (index < maxLen - 1) {
    if (Serial.available() > 0) {
      char c = Serial.read();
      if (c == '\n' || c == '\r') break;
      buffer[index++] = c;
    }
  }
  buffer[index] = '\0';
  return index;
}

bool SerialCommandProcessor::setTemperatureFromInput(const char* input) {
  // Parse the temperature value
  float val = atof(input);
  
  // Validate: atof returns 0.0 for invalid input, so check if it's actually "0"
  bool isValidNumber = (val != 0.0) || (strcmp(input, "0") == 0) || (strcmp(input, "0.0") == 0);
  
  if (!isValidNumber) {
    Serial.println(F("Invalid input. Please enter a number (e.g. 23.0)"));
    return false;
  }
  
  // Validate hard range
  if (val < 0 || val > 50) {
    Serial.println(F("Invalid temperature. Please enter a value between 0 and 50°C."));
    return false;
  }
  
  // Set the threshold
  sensors.setDesiredTempThreshold(val);
  
  // Confirm
  Serial.print(F("Temperature threshold set to "));
  Serial.print(sensors.getDesiredTempThreshold());
  Serial.println(F("°C"));
  
    // Warn if outside suggested range but allow (soft warning)
    if (val < 18.0 || val > 30.0) {
      Serial.print(F("> Note: "));
      Serial.print(val);
      Serial.println(F("°C is outside the typical range."));
    }
  return true;
}

void SerialCommandProcessor::process() {
  // Check if serial data is available
  if (Serial.available() > 0) {
    char command[CMD_BUFFER_SIZE];
    
    // Read input line using helper
    readSerialLine(command, CMD_BUFFER_SIZE);
    
    trim(command);
    toLowerCase(command);
    
    if (strlen(command) == 0) return;
    
    // Parse "set desiredTempThreshold <value>" command
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
        
        if (strEquals(varName, "desiredtempthreshold")) {
          Serial.println();
          setTemperatureFromInput(valueStr);
        } 
        else {
          Serial.println(F("ERROR: Unknown variable. Only 'desiredTempThreshold' can be set."));
        }
      } else {
        Serial.println(F("ERROR: Usage: set desiredTempThreshold <value>"));
      }
    }
    // Parse "status" command
    else if (strEquals(command, "status")) {
      Serial.println(F("\n--- Current Sensor Values ---"));
      Serial.print(F("roomOccupied: ")); Serial.println(sensors.getRoomOccupied() ? F("true") : F("false"));
      Serial.print(F("indoorTemp: ")); Serial.print(sensors.getIndoorTemp()); Serial.println(F("°C"));
      Serial.print(F("outdoorTemp: ")); Serial.print(sensors.getOutdoorTemp()); Serial.println(F("°C"));
      Serial.print(F("indoorHumidity: ")); Serial.print(sensors.getIndoorHumidity()); Serial.println(F("%"));
      Serial.print(F("outdoorHumidity: ")); Serial.print(sensors.getOutdoorHumidity()); Serial.println(F("%"));
      Serial.print(F("sunlightIntense: ")); Serial.println(sensors.getSunlightIntense() ? F("true") : F("false"));
      Serial.print(F("isHotInside: ")); Serial.println(sensors.getIsHotInside() ? F("true") : F("false"));
      Serial.print(F("desiredTempThreshold: ")); Serial.print(sensors.getDesiredTempThreshold()); Serial.println(F("°C"));
    }
    // Parse "help" command
    else if (strEquals(command, "help")) {
      printHelp();
    }
    else {
      Serial.println(F("ERROR: Unknown command. Type 'help' for available commands."));
    }
  }
}

void SerialCommandProcessor::printHelp() {
  Serial.println(F("\n=== Available Commands ==="));
  Serial.println(F("set desiredTempThreshold <value>  - Set temperature threshold (e.g. 24.0)"));
  Serial.println(F("status                            - Show current system status"));
  Serial.println(F("help                              - Show this help"));
  Serial.println(F("==========================="));
}

} // namespace SerialCommand
} // namespace ShadeWave

