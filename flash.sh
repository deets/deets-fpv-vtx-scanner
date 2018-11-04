#!/bin/bash
export PATH=~/esp/xtensa-esp32-elf/bin:$PATH
export IDF_PATH=/home/deets/software/vc/esp-idf
make -j28 flash
