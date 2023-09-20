Depenencies
===========
apt-get install gcc-avr binutils-avr scons avrdude avr-libc

Limitations
===========
- 2K of ram does not allow many nodes and viewd signals
- img is not supported.
- time: date not supported (missing <ctime>)

Disclaimer
==========
This is just a prove of concept to try how small a abmt runtime can get. 
Do not use this as a template for other microcontrollers apart from AVR's. Use the mbed implementation instead. Risc-V is the new 8-bit.

Windows/WSL Notes
=================
https://learn.microsoft.com/en-us/windows/wsl/connect-usb

``usbipd wsl list``

``usbipd wsl attach --busid <busid>``

Implementation Details
======================
- The folder shared-avr shadows the shared-folder that comes with abmt. It contains modifications to get around the litmits of the AVR.
- uses modivied version of avr-stl (https://github.com/andysworkshop/avr-stl from 11.08.2023). uClib+ streams were removed. The libstd
  folder contains modifications that were missing.
