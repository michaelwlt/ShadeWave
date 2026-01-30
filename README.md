# ShadeWave Ctrl Software written for the Arduino Uno

> **Note:** Be sure to install the PlatformIO extension for VS Code to compile and upload this project to your Arduino hardware.

> **Set and Forget:** Once everything is wired up and the code is uploaded to the Arduino, the system is designed to be fully controlable via the serial monitor. No further source code knowledge is required.

## Hardware Wiring

### Sensors
- **PIR Motion Sensor** → Pin D11 (digital)
- **LDR Light Sensor** → Pin A0 (analog)
- **Indoor SHT20 Temperature/Humidity Sensor** (Software I2C):
  - SDA → Pin A1
  - SCL → Pin A2
- **Outdoor SHT20 Temperature/Humidity Sensor** (Hardware I2C):
  - SDA → Pin A4
  - SCL → Pin A5

### Servos
- **Vent Servo** → Pin D9
- **Blinds Servo (Right)** → Pin D10
- **Blinds Servo (Left)** → Pin D8

## Serial Communication

Baud rate: **9600**

Use this baud rate when connecting via serial monitor or sending commands.
