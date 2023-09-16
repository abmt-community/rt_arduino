Depenencies
===========
apt-get install gcc-avr binutils-avr scons avrdude

Limitations
===========
- state machines are not supported
- img is not supported.
- time: date not supported (missing <ctime>)

Disclaimer
==========
This is just a prove of concept to try how small a abmt runtime can get. 
Do not use this as a template for other microcontrollers apart from AVR's. Use the mbed implementation instead. Risc-V is the new 8-bit.

Implementation Details
======================
- The folder shared-avr shadows the shared-folder that comes with abmt. It contains modifications to get around the litmits of the AVR.
- uses modivied version of avr-stl (https://github.com/andysworkshop/avr-stl from 11.08.2023). uClib+ streams were removed. The libstd
  folder contains modifications that were missing.
