default:
	avr-gcc -Os -DF_CPU=160000000UL -mmcu=atmega328p -c -o blinky.o blinky.c
	avr-gcc -o blinky.bin blinky.o
	avr-objcopy -O ihex -R .eeprom blinky.bin blinky.hex
	sudo avrdude -F -V -c arduino -p ATMEGA328P -P /dev/tty.usbmodem1101 -b 115200 -U flash:w:blinky.hex