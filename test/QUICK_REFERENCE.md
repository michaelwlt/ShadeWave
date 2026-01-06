# Quick Reference Card

Quick reference for testing the ShadeWave Smart Environment Control System.

## System Configuration

- **Baud Rate**: 9600
- **Loop Delay**: ~5 seconds
- **Thresholds**:
  - HOT_TEMP_THRESHOLD: 24.0°C
  - HIGH_HUMIDITY_THRESHOLD: 75.0%

## LED Pin Assignments

- **Pin 2**: VENT_OPEN_LED
- **Pin 3**: VENT_CLOSED_LED
- **Pin 4**: BLINDS_OPEN_LED
- **Pin 5**: BLINDS_CLOSED_LED
- **Pin 6**: BLINDS_NEUTRAL_LED

## Serial Commands

### Set Sensor Values
```
set roomOccupied true|false|1|0
set indoorTemp <value>
set outdoorTemp <value>
set indoorHumidity <value>
set outdoorHumidity <value>
set sunlightIntense true|false|1|0
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

## Test Priority Quick Guide

### P0 (Critical) - Test First
- Vent Control: Tests 1.1-1.4
- Blinds Control: Tests 2.1-2.4

### P1 (High) - Test Next
- Threshold boundaries: Tests 1.5, 2.5
- State transitions: Tests 3.1-3.4
- Integration: Tests 4.1-4.4
- LED verification: Tests 6.1-6.2

### P2 (Medium)
- Serial commands: Tests 5.1-5.4
- Initialization: Tests 7.1-7.2
- Edge cases: Tests 8.1-8.3

### P3 (Low)
- Performance: Tests 9.1-9.2

## Common Test Scenarios

### Winter Morning
```
set roomOccupied true
set indoorTemp 18.0
set outdoorTemp 15.0
set outdoorHumidity 60.0
set sunlightIntense true
Expected: Vents OPEN, Blinds OPEN
```

### Summer Afternoon
```
set roomOccupied true
set indoorTemp 26.0
set outdoorTemp 30.0
set outdoorHumidity 45.0
set sunlightIntense true
Expected: Vents CLOSED, Blinds CLOSED
```

### Rainy Day
```
set roomOccupied true
set indoorTemp 22.0
set outdoorTemp 18.0
set outdoorHumidity 85.0
set sunlightIntense false
Expected: Vents CLOSED, Blinds NEUTRAL
```

### Energy Saving (Unoccupied)
```
set roomOccupied false
Expected: Vents CLOSED, Blinds CLOSED
```

## Testing Tips

1. **Wait 5 seconds** between commands for loop iteration
2. **Use `status` command** to verify sensor values
3. **Check LED states** match serial output
4. **First loop iteration** prints everything
5. **Subsequent iterations** only print on changes

## Troubleshooting

| Issue | Solution |
|-------|----------|
| Commands not processed | Check serial connection, baud rate, newline |
| LED states don't match | Verify sensor values with `status` command |
| No serial output | Check serial monitor is open and connected |
| State changes not detected | Wait full 5 seconds, verify values changed |


