# Compiler and Tools
CC = avr-gcc
OBJCOPY = avr-objcopy
AVRDUDE = avrdude

# Microcontroller Configuration
MCU = atmega328p
F_CPU = 16000000UL

# Compilation Flags
CFLAGS = -Os -DF_CPU=$(F_CPU) -mmcu=$(MCU) -c
LDFLAGS = -mmcu=$(MCU)

# Default project
PROJECT ?= blinky

# Source and Output Files
SRCDIR := $(PROJECT)
SRC := $(SRCDIR)/$(PROJECT).c
OBJ := $(SRCDIR)/$(PROJECT).o
BIN := $(SRCDIR)/$(PROJECT).bin
HEX := $(SRCDIR)/$(PROJECT).hex

# Default target
.PHONY: all build upload clean

all:
	@echo "Usage: make build PROJECT=<project_name>"
	@echo "       make upload PROJECT=<project_name>"
	@echo "       make clean PROJECT=<project_name>"

# Build project dynamically
build:
	@echo "Building project: $(PROJECT)"
	$(CC) $(CFLAGS) -o $(OBJ) $(SRC)
	$(CC) $(LDFLAGS) -o $(BIN) $(OBJ)
	$(OBJCOPY) -O ihex -R .eeprom $(BIN) $(HEX)
	@echo "Build complete: $(HEX)"

# Upload to Arduino
upload: build
	@echo "Uploading project: $(PROJECT)"
	sudo $(AVRDUDE) -F -V -c arduino -p $(MCU) -P /dev/tty.usbmodem1101 -b 115200 -U flash:w:$(HEX)
	@echo "Upload complete!"

# Clean project build files
clean:
	@echo "Cleaning project: $(PROJECT)"
	rm -f $(SRCDIR)/*.o $(SRCDIR)/*.bin $(SRCDIR)/*.hex
	@echo "Clean complete!"