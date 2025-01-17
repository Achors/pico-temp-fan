cmake_minimum_required(VERSION 3.13)

set(PROJECT pico-temp-fan)

if (NOT DEFINED ENV{PICO_SDK_PATH})
    message("PICO_SDK_PATH not set, looking for alternative paths")
    if (EXISTS $ENV{HOME}/Pico/pico-sdk)
        set(PICO_SDK_PATH $ENV{HOME}/Pico/pico-sdk)
    elseif (EXISTS $ENV{HOME}/pico/pico-sdk)
        set(PICO_SDK_PATH $ENV{HOME}/pico-sdk)
    elseif (EXISTS $ENV{HOME}/pico-sdk)
        set(PICO_SDK_PATH $ENV{HOME}/pico-sdk)
    else()
        message(FATAL_ERROR "PICO_SDK_PATH not set and SDK not found in default locations")
    endif()
    message("PICO_SDK_PATH set to ${PICO_SDK_PATH}")
else()
    set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})
    message("PICO_SDK_PATH found in ENV and set to ${PICO_SDK_PATH}")
endif()

set(PICO_BOARD "pico_w")
set(ENABLE_WIFI off)

set(PICO_SDK_FETCH_FROM_GIT off)
include(pico_sdk_import.cmake)

project(${PROJECT})

pico_sdk_init()

enable_language(CXX)

add_executable(${PROJECT}
    src/main.c
    src/fan_control.c
    src/rgb_led.c
    src/dht_handler.c
    src/utils.c
    src/buzzer.c
)

target_include_directories(${PROJECT} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/external/rpi-pico-i2c-display-lib
    ${CMAKE_SOURCE_DIR}/pico_dht/dht/include
)

add_subdirectory(pico_dht/dht ${CMAKE_BINARY_DIR}/pico_dht_build)

target_link_libraries(${PROJECT}
    pico_stdlib
    hardware_pwm
    hardware_adc
    hardware_i2c
    pico_rand
    hardware_pio
    dht
)

pico_add_extra_outputs(${PROJECT})
pico_enable_stdio_usb(${PROJECT} 1)
pico_enable_stdio_uart(${PROJECT} 0)
