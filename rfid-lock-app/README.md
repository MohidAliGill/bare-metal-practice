# RFID Lock System (Bare-Metal AVR)

This is a **bare-metal** embedded project written in **C for ATmega328P** that interfaces with the **MFRC522 RFID module** to implement a simple **RFID-based locking system**.

When an authorized tag is presented, the system unlocks (green LED ON), and relocks automatically after 5 seconds (red LED ON).

## Features
- Direct register-level control of:
  - UART
  - SPI
  - GPIO (LEDs)
- Communicates with MFRC522 via SPI
- Detects and verifies RFID card UID
- Unlock/lock logic with LEDs
- Fully implemented without external libraries

## Components Used
- Arduino UNO (ATmega328p)
- MFRCC522 RFID
- RFID Tag
- LEDs

## MFRC Pin Connections
- SDA (SS)      D10 (PB2)
- SCK           D13 (PB5)
- MOSI          D11 (PB3)
- MISO          D12 (PB4)
- RST           D9  (PD9)
- GND           GND
- 3.3v          3.3v

## LED Pin Connections
- Green LED     D6  (PD6)
- Red LED       D7  (PD7)

## How To Build
From the root folder **bare-metal-practice** run the following command in terminal
```make upload PROJECT=rfid-lock-app```

## TODO / Future Improvements
- Replace _delay_ms() with Timer-based non-blocking delay
- Add auto-relock using Timer1 ISR
- Use external interrupt (INT0) for card detection instead of polling
- Add support for multiple authorized UIDs
- Add EEPROM support to store UIDs
- Refactor code into modular files: spi.c, uart.c, mfrc522.c, led.c