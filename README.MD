# Open-NIM Counter firmware
Firmware for the Open-NIM counter module.

## Compilation
In order to compile the program the entirety of the avr-gcc family is required, along with the GNU commands sed, echo, cat, cmake and head. \\
Everything is already configured to produce the final .hex file that contains both the program and the bootloader. In order to compile, create a build folder
```
mkdir build
cd build
```
And then run the cmake command specifying the location of the source code:
```
cmake -DCMAKE_INSTALL_PREFIX=[source code location] .
```
Then the everything has to be compiled (the -j flag for make allows concurrent compilation):
```
make -j
```

## Authors
Leonardo Lisa
Alluringbits

## Licences
Inside the program there are different libraries with different Licences which may not be the same as the GPLv3 licence. The NIM-Counter source code falls under the GPLv3 licence, the U8G2 screen library falls under the BSD licence. The Arduino library falls under the GPLv2.1 licence while both the Wire and SPI libraries fall under the GPLv2 licence.
The STK500v2 bootloader source code falls under the GPLv2 licence. \\
All rights go to the respective authors.
