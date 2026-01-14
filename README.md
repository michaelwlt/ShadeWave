# ShadeWave Ctrl Software written for the Arduino Ono


Make sure to install https://marketplace.cursorapi.com/items/?itemName=pioarduino.pioarduino-ide for 

## System Configuration

- **Baud Rate**: 9600
- **Loop Delay**: ~5 seconds
- **Default Thresholds**:
  - HOT_TEMP_THRESHOLD: 24.0°C
  - HIGH_HUMIDITY_THRESHOLD: 75.0%

## LED Pin Assignments

- **Pin 2**: VENT_OPEN_LED
- **Pin 3**: VENT_CLOSED_LED
- **Pin 4**: BLINDS_OPEN_LED
- **Pin 5**: BLINDS_CLOSED_LED
- **Pin 6**: BLINDS_NEUTRAL_LED

## Motor Pin Assigments 
- **Pin 9**: VENTS_SERVO_PIN
- **Pin 10**: BLINDS_SERVO_PIN

## Sensor Pin Assignments
### Outdoor Sensor
![SHT20 Temperature and Humidity Sensor Arduino Circuit](https://www.circuits-diy.com/wp-content/uploads/2022/12/SHT20-Temperature-and-Humidity-Sensor-Arduino-Circuit.png)

## Serial Commands

### Set Sensor Values
```
set roomOccupied true|false|1|0
set indoorTemp <value>
set outdoorTemp <value>
set indoorHumidity <value>
set outdoorHumidity <value>
set sunlightIntense true|false|1|0
set desiredTempThreshold <value>
set humidityThreshold <value>
```

### Query Commands
```
get          - Show all sensor values
status       - Alias for get
help         - Show help message
```

## Vent Control Logic

| Condition | Result |
|-----------|--------|
| Room unoccupied | CLOSED (energy saving) |
| Occupied + Outdoor < Indoor + Humidity < 75% | OPEN (natural cooling) |
| Occupied + Outdoor < Indoor + Humidity ≥ 75% | CLOSED (high humidity) |
| Occupied + Outdoor ≥ Indoor | CLOSED (keep cool air in) |

## Blinds Control Logic

| Condition | Result |
|-----------|--------|
| Room unoccupied | CLOSED (energy saving) |
| Occupied + No intense sunlight | NEUTRAL |
| Occupied + Intense sunlight + Temp < 24°C | OPEN (winter mode) |
| Occupied + Intense sunlight + Temp ≥ 24°C | CLOSED (summer mode) |

## Troubleshooting

| Issue | Solution |
|-------|----------|
| Commands not processed | Check serial connection, baud rate, newline |
| LED states don't match | Verify sensor values with `status` command |
| No serial output | Check serial monitor is open and connected |
| State changes not detected | Wait full 5 seconds, verify values changed |



