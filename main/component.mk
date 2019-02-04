#
# Main Makefile. This is basically the same as a component makefile.
#
# (Uses default behaviour of compiling all source files in directory, adding 'include' to include path.)


#Compile image file into the resulting firmware binary
#COMPONENT_EMBED_FILES := image.jpg

# app depends on compiled gatt db
ble.o: vtx_scanner_ble.h

# rule to compile gatt db
vtx_scanner_ble.h: $(COMPONENT_PATH)/vtx_scanner_ble.gatt
	$(IDF_PATH)/components/btstack/tool/compile_gatt.py $^ $@

# remove compiled gatt db on clean
COMPONENT_EXTRA_CLEAN = vtx_scanner_ble.h
