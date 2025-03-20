# Bare-Metal Practice

Practicing bare-metal concepts.

## Development Board Used
**Arduino Uno**

## Libraries Required

- **avr-libc**
- **avrdude**
- **binutils-avr**
- **gcc-avr**

On macOS, these can be installed using Homebrew:
```sh
brew install avr-libc avrdude binutils-avr gcc-avr
```

## Building the Examples

From the main project directory (`bare-metal-practice`), use the following `make` commands, replacing `[project-name]` with the actual project name (e.g., `blinky-with-interrupt`).

### **Build the Image**
This command builds the image to be flashed:
```sh
make build PROJECT=[project-name]
```

### **Upload to Arduino**
This command builds and flashes the Arduino.  
**Ensure you update the correct port in the `Makefile`** where the Arduino is connected.
```sh
make upload PROJECT=[project-name]
```

### **Clean Build Files**
This command removes the build files for the selected example:
```sh
make clean PROJECT=[project-name]
```

---

## **Practice Project Examples**

- **blinky**: Forever blinking LED  
- **blinky-with-interrupt**: Blinking an LED without a forever while loop, using interrupts
- **uart**: Send a null terminated string over USART
- **uart-with-buffer**: UART with circular buffer

---
