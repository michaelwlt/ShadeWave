# Test Scenarios for ShadeWave System

This document provides comprehensive test scenarios to validate the Smart Environment Control System's functionality across all components and use cases.

## System Overview

The ShadeWave system controls:
- **Vents**: Based on occupancy, indoor/outdoor temperature comparison, and outdoor humidity
- **Blinds**: Based on occupancy, sunlight intensity, and indoor temperature (summer/winter mode)
- **Sensors**: roomOccupied, indoorTemp, outdoorTemp, indoorHumidity, outdoorHumidity, sunlightIntense
- **Thresholds**: HOT_TEMP_THRESHOLD = 24.0°C, HIGH_HUMIDITY_THRESHOLD = 75.0%

---

## 1. Vent Control Logic Tests

### Test 1.1: Energy Saving Mode (Room Unoccupied)
**Priority**: P0 (Critical)

**Prerequisites**: System initialized and running

**Test Steps**:
1. Send command: `set roomOccupied false`
2. Set any temperature/humidity values
3. Wait for next loop iteration (5 seconds)

**Expected Results**:
- Serial output: "Room not occupied -> Energy Saving Mode"
- Serial output: "Action: Close Vents"
- LED State: VENT_CLOSED_LED (Pin 3) ON, VENT_OPEN_LED (Pin 2) OFF
- System State: Vent = CLOSED

**Pass Criteria**: All expected results match

---

### Test 1.2: Natural Cooling (Ideal Conditions)
**Priority**: P0 (Critical)

**Prerequisites**: System initialized and running

**Test Steps**:
1. Send commands:
   - `set roomOccupied true`
   - `set indoorTemp 25.0`
   - `set outdoorTemp 20.0`
   - `set outdoorHumidity 50.0`
2. Wait for next loop iteration

**Expected Results**:
- Serial output: "Outdoor < Indoor (Cooler Outside)"
- Serial output: "Outdoor humidity is acceptable"
- Serial output: "Action: Open Vents - Natural Cooling"
- LED State: VENT_OPEN_LED (Pin 2) ON, VENT_CLOSED_LED (Pin 3) OFF
- System State: Vent = OPEN

**Pass Criteria**: All expected results match

---

### Test 1.3: High Humidity Block (Cooler Outside but Humid)
**Priority**: P0 (Critical)

**Prerequisites**: System initialized and running

**Test Steps**:
1. Send commands:
   - `set roomOccupied true`
   - `set indoorTemp 25.0`
   - `set outdoorTemp 20.0`
   - `set outdoorHumidity 80.0`
2. Wait for next loop iteration

**Expected Results**:
- Serial output: "Outdoor < Indoor (Cooler Outside)"
- Serial output: "Outdoor humidity is HIGH"
- Serial output: "Action: Keep Vents Closed (high humidity)"
- LED State: VENT_CLOSED_LED (Pin 3) ON, VENT_OPEN_LED (Pin 2) OFF
- System State: Vent = CLOSED

**Pass Criteria**: All expected results match

---

### Test 1.4: Warmer Outside (Keep Cool Air In)
**Priority**: P0 (Critical)

**Prerequisites**: System initialized and running

**Test Steps**:
1. Send commands:
   - `set roomOccupied true`
   - `set indoorTemp 25.0`
   - `set outdoorTemp 28.0`
   - `set outdoorHumidity 60.0`
2. Wait for next loop iteration

**Expected Results**:
- Serial output: "Outdoor > Indoor (Warmer Outside)"
- Serial output: "Action: Close Vents - Keep Cool Air In"
- LED State: VENT_CLOSED_LED (Pin 3) ON, VENT_OPEN_LED (Pin 2) OFF
- System State: Vent = CLOSED

**Pass Criteria**: All expected results match

---

### Test 1.5: Threshold Boundary Tests
**Priority**: P1 (High)

#### Test 1.5.1: Humidity Exactly at Threshold
**Test Steps**:
1. Send commands:
   - `set roomOccupied true`
   - `set indoorTemp 25.0`
   - `set outdoorTemp 20.0`
   - `set outdoorHumidity 75.0`
2. Wait for next loop iteration

**Expected Results**:
- Serial output: "Outdoor humidity is HIGH"
- LED State: VENT_CLOSED_LED ON (threshold is "greater than", so 75.0% should be considered high)

**Pass Criteria**: Vents remain CLOSED

---

#### Test 1.5.2: Humidity Just Below Threshold
**Test Steps**:
1. Send commands:
   - `set roomOccupied true`
   - `set indoorTemp 25.0`
   - `set outdoorTemp 20.0`
   - `set outdoorHumidity 74.9`
2. Wait for next loop iteration

**Expected Results**:
- Serial output: "Outdoor humidity is acceptable"
- LED State: VENT_OPEN_LED ON

**Pass Criteria**: Vents OPEN

---

#### Test 1.5.3: Humidity Just Above Threshold
**Test Steps**:
1. Send commands:
   - `set roomOccupied true`
   - `set indoorTemp 25.0`
   - `set outdoorTemp 20.0`
   - `set outdoorHumidity 75.1`
2. Wait for next loop iteration

**Expected Results**:
- Serial output: "Outdoor humidity is HIGH"
- LED State: VENT_CLOSED_LED ON

**Pass Criteria**: Vents CLOSED

---

## 2. Blinds Control Logic Tests

### Test 2.1: Energy Saving Mode (Room Unoccupied)
**Priority**: P0 (Critical)

**Prerequisites**: System initialized and running

**Test Steps**:
1. Send command: `set roomOccupied false`
2. Set any other sensor values
3. Wait for next loop iteration

**Expected Results**:
- Serial output: "Room not occupied -> Energy Saving Mode"
- Serial output: "Action: Close Blinds"
- LED State: BLINDS_CLOSED_LED (Pin 5) ON, others OFF
- System State: Blinds = CLOSED

**Pass Criteria**: All expected results match

---

### Test 2.2: Summer Mode (Block Heat)
**Priority**: P0 (Critical)

**Prerequisites**: System initialized and running

**Test Steps**:
1. Send commands:
   - `set roomOccupied true`
   - `set sunlightIntense true`
   - `set indoorTemp 26.0`
2. Wait for next loop iteration

**Expected Results**:
- Serial output: "Sunlight is intense"
- Serial output: "Hot inside (Summer Mode)"
- Serial output: "Action: Close Blinds - Block Heat"
- LED State: BLINDS_CLOSED_LED (Pin 5) ON, others OFF
- System State: Blinds = CLOSED

**Pass Criteria**: All expected results match

---

### Test 2.3: Winter Mode (Solar Heating)
**Priority**: P0 (Critical)

**Prerequisites**: System initialized and running

**Test Steps**:
1. Send commands:
   - `set roomOccupied true`
   - `set sunlightIntense true`
   - `set indoorTemp 20.0`
2. Wait for next loop iteration

**Expected Results**:
- Serial output: "Sunlight is intense"
- Serial output: "Not hot inside (Winter Mode)"
- Serial output: "Action: Open Blinds - Solar Heating"
- LED State: BLINDS_OPEN_LED (Pin 4) ON, others OFF
- System State: Blinds = OPEN

**Pass Criteria**: All expected results match

---

### Test 2.4: Neutral Mode (No Intense Sunlight)
**Priority**: P0 (Critical)

**Prerequisites**: System initialized and running

**Test Steps**:
1. Send commands:
   - `set roomOccupied true`
   - `set sunlightIntense false`
   - `set indoorTemp 22.0`
2. Wait for next loop iteration

**Expected Results**:
- Serial output: "Sunlight is not intense"
- Serial output: "Action: Keep Blinds Neutral"
- LED State: BLINDS_NEUTRAL_LED (Pin 6) ON, others OFF
- System State: Blinds = NEUTRAL

**Pass Criteria**: All expected results match

---

### Test 2.5: Threshold Boundary Tests
**Priority**: P1 (High)

#### Test 2.5.1: Temperature Exactly at Threshold
**Test Steps**:
1. Send commands:
   - `set roomOccupied true`
   - `set sunlightIntense true`
   - `set indoorTemp 24.0`
2. Wait for next loop iteration

**Expected Results**:
- Serial output: "Hot inside (Summer Mode)"
- LED State: BLINDS_CLOSED_LED ON (threshold is ">=", so 24.0°C should trigger summer mode)

**Pass Criteria**: Blinds CLOSED

---

#### Test 2.5.2: Temperature Just Below Threshold
**Test Steps**:
1. Send commands:
   - `set roomOccupied true`
   - `set sunlightIntense true`
   - `set indoorTemp 23.9`
2. Wait for next loop iteration

**Expected Results**:
- Serial output: "Not hot inside (Winter Mode)"
- LED State: BLINDS_OPEN_LED ON

**Pass Criteria**: Blinds OPEN

---

#### Test 2.5.3: Temperature Just Above Threshold
**Test Steps**:
1. Send commands:
   - `set roomOccupied true`
   - `set sunlightIntense true`
   - `set indoorTemp 24.1`
2. Wait for next loop iteration

**Expected Results**:
- Serial output: "Hot inside (Summer Mode)"
- LED State: BLINDS_CLOSED_LED ON

**Pass Criteria**: Blinds CLOSED

---

## 3. State Transition Tests

### Test 3.1: Occupancy State Change
**Priority**: P1 (High)

**Prerequisites**: System initialized and running

**Test Steps**:
1. Initial state: `set roomOccupied false` → Wait for loop iteration
2. Verify: Vents CLOSED, Blinds CLOSED
3. Change: `set roomOccupied true`
4. Set favorable conditions:
   - `set indoorTemp 20.0`
   - `set outdoorTemp 18.0`
   - `set outdoorHumidity 50.0`
   - `set sunlightIntense true`
5. Wait for next loop iteration

**Expected Results**:
- State change detected and printed
- Vents transition to OPEN (if conditions allow)
- Blinds transition to OPEN (winter mode)
- Serial output shows state change messages

**Pass Criteria**: State transitions correctly and change is detected

---

### Test 3.2: Temperature Transition (Summer to Winter)
**Priority**: P1 (High)

**Prerequisites**: System initialized and running

**Test Steps**:
1. Initial state:
   - `set roomOccupied true`
   - `set sunlightIntense true`
   - `set indoorTemp 26.0`
   - Wait for loop iteration → Blinds should be CLOSED
2. Change: `set indoorTemp 20.0`
3. Wait for next loop iteration

**Expected Results**:
- `isHotInside` derived value updates from true to false
- Blinds transition from CLOSED to OPEN
- Serial output shows "Not hot inside (Winter Mode)"
- State change detected and printed

**Pass Criteria**: Temperature change triggers correct state transition

---

### Test 3.3: Humidity Transition
**Priority**: P1 (High)

**Prerequisites**: System initialized and running

**Test Steps**:
1. Initial state:
   - `set roomOccupied true`
   - `set indoorTemp 25.0`
   - `set outdoorTemp 20.0`
   - `set outdoorHumidity 50.0`
   - Wait for loop iteration → Vents should be OPEN
2. Change: `set outdoorHumidity 80.0`
3. Wait for next loop iteration

**Expected Results**:
- Vents transition from OPEN to CLOSED
- Serial output shows "Outdoor humidity is HIGH"
- State change detected and printed

**Pass Criteria**: Humidity change triggers correct state transition

---

### Test 3.4: Sunlight Transition
**Priority**: P1 (High)

**Prerequisites**: System initialized and running

**Test Steps**:
1. Initial state:
   - `set roomOccupied true`
   - `set sunlightIntense false`
   - `set indoorTemp 20.0`
   - Wait for loop iteration → Blinds should be NEUTRAL
2. Change: `set sunlightIntense true`
3. Wait for loop iteration → Blinds should be OPEN (winter mode)
4. Change: `set indoorTemp 26.0`
5. Wait for loop iteration → Blinds should be CLOSED (summer mode)

**Expected Results**:
- Blinds transition: NEUTRAL → OPEN → CLOSED
- Each transition is detected and printed
- Correct mode (winter/summer) is applied

**Pass Criteria**: All transitions work correctly

---

## 4. Integration Scenarios

### Test 4.1: Morning Scenario (Winter Day)
**Priority**: P1 (High)

**Prerequisites**: System initialized and running

**Test Steps**:
1. Send commands:
   - `set roomOccupied true`
   - `set indoorTemp 18.0`
   - `set outdoorTemp 15.0`
   - `set outdoorHumidity 60.0`
   - `set sunlightIntense true`
2. Wait for next loop iteration

**Expected Results**:
- Vents: OPEN (natural cooling/heating with low humidity)
- Blinds: OPEN (winter mode - solar heating)
- Serial output confirms both decisions

**Pass Criteria**: Both controllers make correct decisions for winter morning

---

### Test 4.2: Afternoon Scenario (Summer Day)
**Priority**: P1 (High)

**Prerequisites**: System initialized and running

**Test Steps**:
1. Send commands:
   - `set roomOccupied true`
   - `set indoorTemp 26.0`
   - `set outdoorTemp 30.0`
   - `set outdoorHumidity 45.0`
   - `set sunlightIntense true`
2. Wait for next loop iteration

**Expected Results**:
- Vents: CLOSED (warmer outside)
- Blinds: CLOSED (summer mode - block heat)
- Serial output confirms both decisions

**Pass Criteria**: Both controllers make correct decisions for summer afternoon

---

### Test 4.3: Rainy Day Scenario
**Priority**: P1 (High)

**Prerequisites**: System initialized and running

**Test Steps**:
1. Send commands:
   - `set roomOccupied true`
   - `set indoorTemp 22.0`
   - `set outdoorTemp 18.0`
   - `set outdoorHumidity 85.0`
   - `set sunlightIntense false`
2. Wait for next loop iteration

**Expected Results**:
- Vents: CLOSED (high humidity outside)
- Blinds: NEUTRAL (no intense sunlight)
- Serial output confirms both decisions

**Pass Criteria**: Both controllers make correct decisions for rainy day

---

### Test 4.4: Evening Scenario (Room Vacant)
**Priority**: P1 (High)

**Prerequisites**: System initialized and running

**Test Steps**:
1. Send command: `set roomOccupied false`
2. Set any temperature/humidity values
3. Wait for next loop iteration

**Expected Results**:
- Vents: CLOSED (energy saving)
- Blinds: CLOSED (energy saving)
- Serial output shows energy saving mode for both

**Pass Criteria**: Both controllers enter energy saving mode

---

### Test 4.5: Full Day Cycle Test
**Priority**: P2 (Medium)

**Prerequisites**: System initialized and running

**Test Steps**:
1. **6:00 AM** - `set roomOccupied false` → Wait, verify CLOSED/CLOSED
2. **8:00 AM** - `set roomOccupied true`, `set indoorTemp 18.0`, `set outdoorTemp 15.0`, `set outdoorHumidity 60.0`, `set sunlightIntense true` → Wait, verify OPEN/OPEN
3. **12:00 PM** - `set indoorTemp 26.0`, `set outdoorTemp 30.0`, `set sunlightIntense true` → Wait, verify CLOSED/CLOSED
4. **6:00 PM** - `set roomOccupied false` → Wait, verify CLOSED/CLOSED
5. **8:00 PM** - `set roomOccupied true`, `set indoorTemp 22.0`, `set outdoorTemp 18.0`, `set outdoorHumidity 50.0`, `set sunlightIntense false` → Wait, verify OPEN/NEUTRAL

**Expected Results**:
- Each time period shows correct state transitions
- System responds appropriately to each scenario

**Pass Criteria**: All time periods handled correctly

---

## 5. Serial Command Interface Tests

### Test 5.1: Valid Commands
**Priority**: P2 (Medium)

#### Test 5.1.1: Boolean Commands
**Test Steps**:
1. `set roomOccupied true` → Verify "OK: roomOccupied set to true"
2. `set roomOccupied false` → Verify "OK: roomOccupied set to false"
3. `set roomOccupied 1` → Verify "OK: roomOccupied set to true"
4. `set roomOccupied 0` → Verify "OK: roomOccupied set to false"
5. `set sunlightIntense true` → Verify "OK: sunlightIntense set to true"
6. `set sunlightIntense false` → Verify "OK: sunlightIntense set to false"

**Pass Criteria**: All commands accepted with correct confirmation

---

#### Test 5.1.2: Float Commands
**Test Steps**:
1. `set indoorTemp 25.5` → Verify "OK: indoorTemp set to 25.50"
2. `set outdoorTemp 20.0` → Verify "OK: outdoorTemp set to 20.00"
3. `set indoorHumidity 60.0` → Verify "OK: indoorHumidity set to 60.00"
4. `set outdoorHumidity 75.5` → Verify "OK: outdoorHumidity set to 75.50"

**Pass Criteria**: All commands accepted with correct confirmation

---

#### Test 5.1.3: Query Commands
**Test Steps**:
1. `get` → Verify all sensor values displayed with units
2. `status` → Verify all sensor values displayed (alias for get)
3. `help` → Verify help message displayed

**Pass Criteria**: All query commands work correctly

---

### Test 5.2: Invalid Commands
**Priority**: P2 (Medium)

**Test Steps**:
1. `set roomOccupied maybe` → Should show "ERROR: Invalid value. Use 'true' or 'false'"
2. `set indoorTemp abc` → Should show "ERROR: Invalid temperature value"
3. `set unknownVar 123` → Should show "ERROR: Unknown variable name"
4. `invalidCommand` → Should show "ERROR: Unknown command. Type 'help' for available commands."
5. `set` → Should show "ERROR: Invalid command format. Use: set <variable> <value>"
6. `set roomOccupied` → Should show "ERROR: Invalid command format. Use: set <variable> <value>"

**Pass Criteria**: All invalid commands rejected with appropriate error messages

---

### Test 5.3: Command Parsing Edge Cases
**Priority**: P2 (Medium)

**Test Steps**:
1. `SET roomOccupied TRUE` (uppercase) → Should work (case-insensitive)
2. `  set   roomOccupied   true  ` (extra spaces) → Should trim and work
3. `set roomOccupied true\n` (with newline) → Should work
4. Send command >64 characters → Should handle gracefully (may truncate)

**Pass Criteria**: Edge cases handled correctly

---

### Test 5.4: Command Response Verification
**Priority**: P2 (Medium)

**Test Steps**:
1. Send `set indoorTemp 25.0`
2. Immediately send `get`
3. Verify temperature is 25.0
4. Wait for next loop iteration
5. Verify state changes reflect new temperature

**Pass Criteria**: Commands processed immediately and state updates correctly

---

## 6. LED Output Tests

### Test 6.1: LED State Verification
**Priority**: P1 (High)

**Test Steps**:
For each controller state, verify correct LED is ON and others are OFF:

1. **Vent OPEN**:
   - Set conditions for vent open
   - Verify: VENT_OPEN_LED (Pin 2) ON, VENT_CLOSED_LED (Pin 3) OFF

2. **Vent CLOSED**:
   - Set conditions for vent closed
   - Verify: VENT_OPEN_LED (Pin 2) OFF, VENT_CLOSED_LED (Pin 3) ON

3. **Blinds OPEN**:
   - Set conditions for blinds open
   - Verify: BLINDS_OPEN_LED (Pin 4) ON, others OFF

4. **Blinds CLOSED**:
   - Set conditions for blinds closed
   - Verify: BLINDS_CLOSED_LED (Pin 5) ON, others OFF

5. **Blinds NEUTRAL**:
   - Set conditions for blinds neutral
   - Verify: BLINDS_NEUTRAL_LED (Pin 6) ON, others OFF

**Pass Criteria**: Only correct LED is ON for each state

---

### Test 6.2: LED Transition Test
**Priority**: P1 (High)

**Test Steps**:
1. Set initial state (e.g., Blinds OPEN)
2. Verify LED state
3. Change conditions to trigger state change (e.g., Blinds CLOSED)
4. Wait for loop iteration
5. Verify LED transition is clean (no flickering)
6. Verify only one LED per controller is ON at any time

**Pass Criteria**: Clean transitions with no flickering, only one LED ON per controller

---

## 7. System Initialization Tests

### Test 7.1: Startup Sequence
**Priority**: P2 (Medium)

**Prerequisites**: System power-on or reset

**Test Steps**:
1. Power on or reset the system
2. Observe serial output
3. Observe LED behavior

**Expected Results**:
- Serial output: "Smart Environment Control System Starting..."
- Serial output: "Testing all LEDs..."
- Each LED tested for 2 seconds in sequence:
  - Pin 2 (Vent Open)
  - Pin 3 (Vent Closed)
  - Pin 4 (Blinds Open)
  - Pin 5 (Blinds Closed)
  - Pin 6 (Blinds Neutral)
- Serial output: "LED test complete. Starting system in 3 seconds..."
- Serial output: "System initialized. Starting control loop..."
- Help message displayed
- System enters main loop

**Pass Criteria**: All initialization steps complete correctly

---

### Test 7.2: First Run Behavior
**Priority**: P2 (Medium)

**Prerequisites**: System just initialized

**Test Steps**:
1. After initialization, observe first loop iteration
2. Observe subsequent loop iterations

**Expected Results**:
- First loop iteration:
  - Prints all sensor readings (forcePrint)
  - Prints all controller statuses (forcePrint)
  - Prints system state (forcePrint)
- Subsequent iterations:
  - Only prints when sensor values change
  - Only prints when controller states change
  - Only prints when system state changes

**Pass Criteria**: First run prints everything, subsequent runs only print changes

---

## 8. Edge Cases and Error Conditions

### Test 8.1: Extreme Values
**Priority**: P2 (Medium)

**Test Steps**:
1. `set indoorTemp -10.0` → Verify system handles very cold temperature
2. `set indoorTemp 50.0` → Verify system handles very hot temperature
3. `set outdoorHumidity 0.0` → Verify system handles very dry air
4. `set outdoorHumidity 100.0` → Verify system handles saturated air
5. `set outdoorTemp 25.0`, `set indoorTemp 25.0` → Verify system handles equal temperatures

**Expected Results**:
- System handles extreme values without crashing
- Logic still works correctly (e.g., equal temps → vents closed)

**Pass Criteria**: System handles extreme values gracefully

---

### Test 8.2: Rapid State Changes
**Priority**: P2 (Medium)

**Test Steps**:
1. Rapidly send multiple commands:
   - `set roomOccupied true`
   - `set roomOccupied false`
   - `set roomOccupied true`
   - `set indoorTemp 20.0`
   - `set indoorTemp 26.0`
   - `set indoorTemp 20.0`
2. Wait for loop iteration
3. Verify final state is correct

**Expected Results**:
- System handles rapid changes gracefully
- Final state is correct (last valid command)
- No state corruption

**Pass Criteria**: System handles rapid changes without issues

---

### Test 8.3: Concurrent Sensor Updates
**Priority**: P2 (Medium)

**Test Steps**:
1. Send multiple commands in quick succession:
   - `set roomOccupied true`
   - `set indoorTemp 26.0`
   - `set outdoorTemp 20.0`
   - `set outdoorHumidity 50.0`
   - `set sunlightIntense true`
2. Wait for loop iteration
3. Verify all updates processed
4. Verify `isHotInside` derived value is correct
5. Verify controllers respond to all changes

**Pass Criteria**: All updates processed and reflected correctly

---

## 9. Performance and Reliability Tests

### Test 9.1: Long-Running Test
**Priority**: P3 (Low)

**Test Steps**:
1. Run system for extended period (1+ hour)
2. Periodically send commands
3. Monitor serial output
4. Check for memory issues or state corruption

**Expected Results**:
- No memory leaks (if monitoring available)
- State tracking remains accurate
- Serial command processing remains responsive
- No crashes or hangs

**Pass Criteria**: System runs stably for extended period

---

### Test 9.2: Loop Timing Test
**Priority**: P3 (Low)

**Test Steps**:
1. Note timestamp of loop iteration start
2. Wait for next loop iteration
3. Calculate time difference
4. Send commands during delay period
5. Verify commands are processed during delay

**Expected Results**:
- Loop delay is approximately 5 seconds (±0.5s acceptable)
- Commands can be processed during delay
- System remains responsive

**Pass Criteria**: Loop timing is correct and system remains responsive

---

## Test Execution Checklist

Use this checklist to track test execution:

- [ ] Section 1: Vent Control Logic Tests (5 tests)
- [ ] Section 2: Blinds Control Logic Tests (5 tests)
- [ ] Section 3: State Transition Tests (4 tests)
- [ ] Section 4: Integration Scenarios (5 tests)
- [ ] Section 5: Serial Command Interface Tests (4 test groups)
- [ ] Section 6: LED Output Tests (2 tests)
- [ ] Section 7: System Initialization Tests (2 tests)
- [ ] Section 8: Edge Cases and Error Conditions (3 tests)
- [ ] Section 9: Performance and Reliability Tests (2 tests)

---

## Test Result Template

For each test, document:

```
Test ID: [e.g., 1.1]
Test Name: [Test name]
Date: [Date]
Tester: [Name]

Prerequisites: [Initial state]
Test Steps: [Steps taken]
Expected Results: [What should happen]
Actual Results: [What actually happened]

Pass/Fail: [PASS / FAIL]
Notes: [Any observations or issues]
```

---

## Priority Summary

- **P0 (Critical)**: Tests 1.1-1.4, 2.1-2.4 - Core functionality
- **P1 (High)**: Tests 1.5, 2.5, 3.1-3.4, 4.1-4.4, 6.1-6.2 - State transitions, boundaries
- **P2 (Medium)**: Tests 4.5, 5.1-5.4, 7.1-7.2, 8.1-8.3 - Commands, edge cases
- **P3 (Low)**: Tests 9.1-9.2 - Performance, long-running

