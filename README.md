# Seeed XIAO RP2040 v1.22 HID mode detector

## behavior

| USER LED COLOR | MEANING |
|----|----|
| RED | NO USB CONNECTION |
| GREEN | BOOT MODE |
| BLUE | REPORT MODE |

* When you press `boot` button, xiao sends `DOWN_ARROW` key and sends `Hello, World!` message to USB serial port.

## how to build

Set `PICO_SDK_PATH` environment variable and `mkdir build && cd build && cmake ../ && make`


