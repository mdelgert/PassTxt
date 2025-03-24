#!/bin/bash
cd ..
#pio run
#esptool.py erase_flash
#rm -rf data/www/node_modules

pio run --target buildfs
pio run --target uploadfs
pio run --target upload

#pio remote --agent pve1 run --target upload
#pio remote --agent d1 run --target upload
#esptool.py --chip esp32-s3 --port /dev/ttyACM0 --baud 921600 write_flash 0x1000 ../.pio/build/esp32-s3-devkitc-1/device.bin