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
bool roomOccupied = true;           // Motion sensor: true = occupied, false = not occupied
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

void readSensorData() {
  // In a real system, this would read from actual sensors
  // For now, we use dummy values that can be modified for testing
  
  // You can modify these values here to test different scenarios:
  roomOccupied = true;        // Change to false to test energy saving mode
  indoorTemp = 25.0;         // Change to test temperature comparisons
  outdoorTemp = 20.0;        // Change to test temperature comparisons
  indoorHumidity = 50.0;
  outdoorHumidity = 70.0;    // Change to >75 to test high humidity
  sunlightIntense = true;    // Change to test sunlight control
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
  delay(5000); // Wait 5 seconds before next sensor reading
}
