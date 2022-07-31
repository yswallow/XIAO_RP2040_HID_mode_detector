cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(my_project)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

add_executable(XIAO_USB_DETECTOR)

target_sources(XIAO_USB_DETECTOR PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/main.c
        ${CMAKE_CURRENT_LIST_DIR}/usb_descriptors.c
        )

# Make sure TinyUSB can find tusb_config.h
target_include_directories(XIAO_USB_DETECTOR PUBLIC
        ${CMAKE_CURRENT_LIST_DIR})

# In addition to pico_stdlib required for common PicoSDK functionality, add dependency on tinyusb_device
# for TinyUSB device support and tinyusb_board for the additional board support library used by the example
target_link_libraries(XIAO_USB_DETECTOR PUBLIC hardware_clocks pico_stdlib tinyusb_device tinyusb_board)

pico_add_extra_outputs(XIAO_USB_DETECTOR)

# add url via pico_set_program_url
#example_auto_set_url(XIAO_USB_DETECTOR)
