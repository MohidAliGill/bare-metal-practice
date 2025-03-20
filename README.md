# bare-metal-practice

Practicing bare metal concepts

Development Board used: Arduino Uno

## Libraries Required
avr-libc
avrdude 
binutils-avr 
gcc-avr

On mac-os can be installed using `brew`

## Building the examples

From the main project directory i.e `bare-metal-practice` use the following `make` commands
replacing [example-name] with the example to build e.g: blinky-with-interrupt

```make build PROJECT=[project-name]```
This command will build the image to be flashed

```make upload PROJECT=[project-name]```
This command will build and flash the Arduino but make sure to change the port in `Makefile` that Arduino is connected to

```make clean PROJECT=[project-name]```
This command cleans the build files for the selected example

## Practice Projects Examples

    - blinky: forever blinking LED
    - blinky-with-interrupt: blinking an LED without a forever while loop and using interrupts