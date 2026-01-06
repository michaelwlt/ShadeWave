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

void SerialCommandProcessor::process() {
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
            sensors.setRoomOccupied(true);
            Serial.println(F("OK: roomOccupied set to true"));
          } else if (strEquals(valueStr, "false") || strEquals(valueStr, "0")) {
            sensors.setRoomOccupied(false);
            Serial.println(F("OK: roomOccupied set to false"));
          } else {
            Serial.println(F("ERROR: Invalid value. Use 'true' or 'false'"));
          }
        }
        else if (strEquals(varName, "sunlightintense")) {
          if (strEquals(valueStr, "true") || strEquals(valueStr, "1")) {
            sensors.setSunlightIntense(true);
            Serial.println(F("OK: sunlightIntense set to true"));
          } else if (strEquals(valueStr, "false") || strEquals(valueStr, "0")) {
            sensors.setSunlightIntense(false);
            Serial.println(F("OK: sunlightIntense set to false"));
          } else {
            Serial.println(F("ERROR: Invalid value. Use 'true' or 'false'"));
          }
        }
        // Parse float values
        else if (strEquals(varName, "indoortemp")) {
          float val = atof(valueStr);
          if (val != 0.0 || strEquals(valueStr, "0") || strEquals(valueStr, "0.0")) {
            sensors.setIndoorTemp(val);
            Serial.print(F("OK: indoorTemp set to "));
            Serial.println(sensors.getIndoorTemp());
          } else {
            Serial.println(F("ERROR: Invalid temperature value"));
          }
        }
        else if (strEquals(varName, "outdoortemp")) {
          float val = atof(valueStr);
          if (val != 0.0 || strEquals(valueStr, "0") || strEquals(valueStr, "0.0")) {
            sensors.setOutdoorTemp(val);
            Serial.print(F("OK: outdoorTemp set to "));
            Serial.println(sensors.getOutdoorTemp());
          } else {
            Serial.println(F("ERROR: Invalid temperature value"));
          }
        }
        else if (strEquals(varName, "indoorhumidity")) {
          float val = atof(valueStr);
          if (val != 0.0 || strEquals(valueStr, "0") || strEquals(valueStr, "0.0")) {
            sensors.setIndoorHumidity(val);
            Serial.print(F("OK: indoorHumidity set to "));
            Serial.println(sensors.getIndoorHumidity());
          } else {
            Serial.println(F("ERROR: Invalid humidity value"));
          }
        }
        else if (strEquals(varName, "outdoorhumidity")) {
          float val = atof(valueStr);
          if (val != 0.0 || strEquals(valueStr, "0") || strEquals(valueStr, "0.0")) {
            sensors.setOutdoorHumidity(val);
            Serial.print(F("OK: outdoorHumidity set to "));
            Serial.println(sensors.getOutdoorHumidity());
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
    else if (strEquals(command, "status")) {
      Serial.println(F("\n--- Current Sensor Values ---"));
      Serial.print(F("roomOccupied: ")); Serial.println(sensors.getRoomOccupied() ? F("true") : F("false"));
      Serial.print(F("indoorTemp: ")); Serial.print(sensors.getIndoorTemp()); Serial.println(F("°C"));
      Serial.print(F("outdoorTemp: ")); Serial.print(sensors.getOutdoorTemp()); Serial.println(F("°C"));
      Serial.print(F("indoorHumidity: ")); Serial.print(sensors.getIndoorHumidity()); Serial.println(F("%"));
      Serial.print(F("outdoorHumidity: ")); Serial.print(sensors.getOutdoorHumidity()); Serial.println(F("%"));
      Serial.print(F("sunlightIntense: ")); Serial.println(sensors.getSunlightIntense() ? F("true") : F("false"));
      Serial.print(F("isHotInside: ")); Serial.println(sensors.getIsHotInside() ? F("true") : F("false"));
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

} // namespace SerialCommand
} // namespace ShadeWave

