# ShadeWave - Smart Environment Control System

An Arduino Uno program that controls a smart environment based on sensor data, following a decision flowchart for vent and blinds control.

## Features

- **Occupancy-based control**: Energy saving mode when room is unoccupied
- **Temperature-based vent control**: Natural cooling when outdoor air is cooler and humidity is acceptable
- **Sunlight-based blinds control**: Adjusts blinds based on sunlight intensity and indoor temperature (summer/winter modes)

## Hardware Requirements

- Arduino Uno
- 5 LEDs (with appropriate resistors, typically 220Ω)
- Jumper wires
- Breadboard (optional, for easier wiring)

## Wiring Diagram

Connect the LEDs as follows:

### Vent Control LEDs
- **Pin 2** → LED (with 220Ω resistor) → GND (Vent Open indicator)
- **Pin 3** → LED (with 220Ω resistor) → GND (Vent Closed indicator)

### Blinds Control LEDs
- **Pin 4** → LED (with 220Ω resistor) → GND (Blinds Open indicator)
- **Pin 5** → LED (with 220Ω resistor) → GND (Blinds Closed indicator)
- **Pin 6** → LED (with 220Ω resistor) → GND (Blinds Neutral indicator)

**Note**: Always use current-limiting resistors (220Ω recommended) in series with LEDs to prevent damage.

## System Logic

The system follows this decision flowchart:

1. **Read Sensor Data** (currently using dummy values)
2. **Check Occupancy**:
   - If not occupied → Energy Saving Mode (close vents & blinds)
   - If occupied → Continue to temperature and sunlight checks

3. **Vent Control** (when occupied):
   - Compare indoor vs outdoor temperature
   - If outdoor < indoor (cooler outside):
     - Check outdoor humidity
     - If humidity is low → Open vents (natural cooling)
     - If humidity is high → Keep vents closed
   - If outdoor > indoor (warmer outside) → Close vents (keep cool air in)

4. **Blinds Control** (when occupied):
   - Check sunlight intensity
   - If intense:
     - If hot inside (summer) → Close blinds (block heat)
     - If not hot (winter) → Open blinds (solar heating)
   - If not intense → Keep blinds neutral

## Testing Different Scenarios

You can modify the dummy sensor values in the `readSensorData()` function to test different scenarios:

```cpp
// Test Energy Saving Mode
roomOccupied = false;

// Test Natural Cooling (outdoor cooler, low humidity)
indoorTemp = 25.0;
outdoorTemp = 20.0;
outdoorHumidity = 50.0;

// Test High Humidity (should keep vents closed)
outdoorTemp = 20.0;
outdoorHumidity = 80.0;

// Test Summer Mode (intense sun, hot inside)
sunlightIntense = true;
indoorTemp = 26.0;  // Above HOT_TEMP_THRESHOLD (24°C)

// Test Winter Mode (intense sun, cool inside)
sunlightIntense = true;
indoorTemp = 20.0;  // Below HOT_TEMP_THRESHOLD
```

## Serial Monitor Output

The program outputs detailed information to the Serial Monitor (9600 baud):
- Sensor readings
- Decision logic steps
- Actions taken
- Current system state

## Configuration

You can adjust these thresholds in the code:

- `HIGH_HUMIDITY_THRESHOLD`: 75.0% (humidity considered "high")
- `HOT_TEMP_THRESHOLD`: 24.0°C (temperature considered "hot" for summer mode)
- Loop delay: 5000ms (5 seconds between sensor readings)

## Building and Uploading

1. Open the project in PlatformIO
2. Connect your Arduino Uno
3. Build and upload the project
4. Open Serial Monitor at 9600 baud to see the system output
5. Observe the LEDs to see the current state

## Future Enhancements

To make this a real system, replace dummy values with actual sensor readings:
- Motion sensor (PIR) for occupancy detection
- Temperature sensors (DHT22, DS18B20, etc.)
- Humidity sensors (DHT22, etc.)
- Light sensor (photoresistor or BH1750)

