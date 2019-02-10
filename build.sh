#!/bin/bash

export PATH=~/esp/xtensa-esp32-elf/bin:$PATH
export IDF_PATH=~/software/vc/esp-idf-3.1.2

if [ `uname` == 'Darwin' ]; then
    CORES=4
    export PATH="/Users/deets/.virtualenvs/new-joy-tracker-4aTHN6M1/bin:$PATH"
else
    CORES=28
fi

make -j$CORES
