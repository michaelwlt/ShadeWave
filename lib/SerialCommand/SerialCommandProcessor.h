#ifndef SERIAL_COMMAND_PROCESSOR_H
#define SERIAL_COMMAND_PROCESSOR_H

#include <Arduino.h>

namespace ShadeWave {
namespace Sensor {
  class SensorData;
}

namespace SerialCommand {

class SerialCommandProcessor {
private:
  Sensor::SensorData& sensors;
  static const int CMD_BUFFER_SIZE;
  
  // Helper functions
  void toLowerCase(char* str);
  void trim(char* str);
  bool strEquals(const char* a, const char* b);

public:
  SerialCommandProcessor(Sensor::SensorData& sensors);
  
  void runSetupWizard();  // Blocking setup for first-time configuration
  void process();
  void printHelp();
};

} // namespace SerialCommand
} // namespace ShadeWave

#endif // SERIAL_COMMAND_PROCESSOR_H

