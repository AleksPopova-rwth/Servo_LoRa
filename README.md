# Servo_LoRa

This repository contains the code and documentation for controlling a servo motor using LoRa (Long Range) communication technology. It demonstrates how to transmit and receive commands via LoRa to adjust the position of a servo motor.

## Features

- **LoRa Communication:** Utilizes LoRa modules for long-range wireless communication.
- **Servo Control:** Adjusts the servo motor's position based on received commands.
- **Efficient Power Usage:** Optimized for low power consumption during operation.

## Components

### Hardware
- **2 LoRa Modules** (SX1280) - 3.3V
- **Microcontrollers** (STM32446re)
- **Servo Motor** - 5 Volt.
- **Power Supply**
- **Connecting Wires**

### Software
- STM32CubeIDE

## Setup

1. **Hardware Connections:**
   - Connect the LoRa module to the microcontroller's SPI pins.
   - Connect the servo motor to a PWM-capable pin on the microcontroller.
   - Ensure the power supply meets the requirements of both the microcontroller and the servo motor.

2. **Software Configuration:**
   - Clone this repository:
     ```bash
     git clone https://github.com/AleksPopova-rwth/Servo_LoRa.git
     ```
   - Open the project in your preferred IDE.
   - Install the necessary libraries for LoRa and servo control.

3. **Upload Code:**
   - Modify the configuration in `config.h` (if applicable) for your specific hardware setup.
   - Compile and upload the code to your microcontroller.

## Usage

1. Power on the system.
2. Send commands via the LoRa transmitter to control the servo motor.
3. Observe the servo motor adjust its position in response to received commands.

## Examples

### Sending a Command
To send a command to set the servo position to 90 degrees, transmit the following data via LoRa:
```
uint8_t data_180[PACKET_SIZE] = {90};  // Beispiel-Daten
```

### Receiving and Adjusting
The receiver will decode the command and adjust the servo accordingly.

## Troubleshooting

- **No Response from Servo:**
  - Check the power supply connections.
  - Verify the servo motor's PWM signal.

- **LoRa Communication Issues:**
  - Ensure the LoRa modules are within range.
  - Check SPI connections and configurations.


## Contact
For any questions, feel free to reach out via email: alexpopova366@gmail.com.

---

Happy building! 🎉
