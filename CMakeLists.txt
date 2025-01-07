cmake_minimum_required(VERSION 3.13)

# TODO: Set your project name here:
set(PROJECT pico-temp-fan)

# Handle PICO_SDK_PATH
if (NOT DEFINED ENV{PICO_SDK_PATH})
    message("PICO_SDK_PATH not set, looking for alternative paths")
    if (EXISTS $ENV{HOME}/Pico/pico-sdk)
        set(PICO_SDK_PATH $ENV{HOME}/Pico/pico-sdk)
    elseif (EXISTS $ENV{HOME}/pico/pico-sdk)
        set(PICO_SDK_PATH $ENV{HOME}/pico/pico-sdk)
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

# Initialize Pico SDK
set(PICO_SDK_FETCH_FROM_GIT off)
include(pico_sdk_import.cmake)

project(${PROJECT})

# Initialize the SDK
pico_sdk_init()

# File collection (ensure this is defined before add_executable)
file(GLOB SOURCES 
    "${CMAKE_SOURCE_DIR}/*.c"  # Include all .c files in the source directory
    "${CMAKE_SOURCE_DIR}/*.h"  # Include all .h files in the source directory
)

# Create executable target
add_executable(${PROJECT} ${SOURCES})

# Specify include directories for the target after defining it
target_include_directories(${PROJECT} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${PICO_SDK_PATH}/include
)

# Compiler options
add_compile_options(-Wall -Wno-format -Wno-unused-function -Wno-maybe-uninitialized)

# Link required libraries
target_link_libraries(${PROJECT}
    pico_stdlib
    hardware_pwm
    hardware_adc
    hardware_i2c
    pico_rand
    hardware_pio
    dht
)

# Additional subdirectory for DHT library
add_subdirectory(pico_dht/dht ${CMAKE_BINARY_DIR}/pico_dht_build)

# Generate map, bin, and hex files
pico_add_extra_outputs(${PROJECT})

# Enable USB output and disable UART
pico_enable_stdio_usb(${PROJECT} 1)
pico_enable_stdio_uart(${PROJECT} 0)

# Optional: WiFi configuration if enabled
if (${ENABLE_WIFI} AND PICO_CYW43_SUPPORTED)
    add_compile_definitions(WIFI)
    target_link_libraries(${PROJECT} pico_cyw43_arch_none)
endif()
