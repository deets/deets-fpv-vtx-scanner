#!/bin/bash

BASE=$(dirname $0)
common=$BASE/common.sh
if [ ! -e $common ]; then
    echo "ERROR: couldn't locate common.sh at $common!"
    exit 1
fi

. $common

echo flash to $ESPPORT with $CORES
make ESPBAUD=$ESPBAUD ESPPORT=$ESPPORT -j$CORES flash
