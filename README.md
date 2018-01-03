# HCS12SS59T AVR Library
An AVR C Library for the HCS12SS59T vacuum fluorescent display.

## Files
* ```hcs12ss59t.h```, ```hcs12ss59t.c```: Display library (only basic functionality)
* ```vfd_fun.c```, ```vfd_fun.h```, ```main.c```: Demo code. ```vfd_fun``` just provides some transition effects.

## Usage
0) Include ```hcs12ss59t.h```, and add ```hcs12ss59t.c``` to your source file list
1) Call ```hcs12ss59t_init()```
2) Call ```hcs12ss59t_set_text(<12 ch. ASCII string>)``` to ASCII text or ```hcs12ss59t_set_buffer()``` to set the raw data according to the display's codes.
3) Call ```hcs12ss59t_set_brightness(<0 - 15>)``` to adjust the brightness

## Compiling the demo
1) Check the processor and clock speed in the Makefile
2) Run ```make```, this will generate the main.hex file
3) Run ```make program``` or flash the hex file manually

## Hardware
Thanks to qrti for his research into this display. You can find info on how to connect and drive the display on his GitHub project. It also includes an Arduino library.
https://github.com/qrti/VFD-HCS-12SS59T
