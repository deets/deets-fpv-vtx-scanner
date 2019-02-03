#!/bin/bash
export PATH=~/esp/xtensa-esp32-elf/bin:$PATH
export IDF_PATH=~/software/vc/esp-idf

BAUD=460800

if [ `uname` == 'Darwin' ]; then
    CORES=4
    PORT=/dev/tty.SLAB_USBtoUART
else
    PORT=/dev/ttyUSB0
    CORES=28
fi


make ESPBAUD=$BAUD ESPPORT=$PORT -j$CORES flash
