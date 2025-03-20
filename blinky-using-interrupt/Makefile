default:
	avr-gcc -Os -DF_CPU=160000000UL -mmcu=atmega328p -c -o blinky-with-interrupt.o blinky-with-interrupt.c
	avr-gcc -o blinky-with-interrupt.bin blinky-with-interrupt.o
	avr-objcopy -O ihex -R .eeprom blinky-with-interrupt.bin blinky-with-interrupt.hex
	sudo avrdude -F -V -c arduino -p ATMEGA328P -P /dev/tty.usbmodem1101 -b 115200 -U flash:w:blinky-with-interrupt.hex