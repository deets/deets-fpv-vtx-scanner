#!/bin/bash

export PATH=~/esp/xtensa-esp32-elf/bin:$PATH
export IDF_PATH=~/software/vc/esp-idf

if [ `uname` == 'Darwin' ]; then
    CORES=4
else
    CORES=28
fi

make -j$CORES
