for sdk_candidate in ~/esp/xtensa-esp32-elf/bin; do
    if [ -e $sdk_candidate ];then
        PATH=$sdk_candidate:$PATH
        break
    fi
done

for idf_canditate in ~/software/vc/esp-idf-3.1.2 ~/software/vc/esp-idf; do
    if [ -e $idf_canditate ]; then
        IDF_PATH=$idf_canditate
        break
    fi
done

if [ -z ${ESPPORT} ]; then
    ports=(/dev/ttyUSB{0,1,2})
    for candidate in /dev/tty.SLAB_USBtoUART $ports; do
        if [ -e $candidate ]; then
            ESPPORT=$candidate
        fi
    done
fi

if [ -z ${ESPBAUD} ]; then
    ESPBAUD=460800
fi


if [ `uname` == 'Darwin' ]; then
    CORES=4
else
    CORES=28
fi

export PATH
export IDF_PATH
export ESPPORT
export ESPBAUD
