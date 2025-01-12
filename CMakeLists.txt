cmake_minimum_required(VERSION 3.13)

# TODO: Set your project name here:
set(PROJECT pico-temp-fan)

# Handle PICO_SDK_PATH
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

# Initialize Pico SDK
set(PICO_SDK_FETCH_FROM_GIT off)
include(pico_sdk_import.cmake)

project(${PROJECT})

# Initialize the SDK
pico_sdk_init()

# Enable C++ in CMake (needed for pi-pico-LCD)
enable_language(CXX)

# File collection for C++ files
file(GLOB SOURCES 
    "${CMAKE_SOURCE_DIR}/*.cpp"  # Include all .cpp files in the source directory
    "${CMAKE_SOURCE_DIR}/*.h"    # Include all .h files in the source directory
)

# Create executable target
add_executable(${PROJECT} ${SOURCES})

# Specify include directories for the target after defining it
target_include_directories(${PROJECT} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${PICO_SDK_PATH}/include
    ${CMAKE_SOURCE_DIR}/external/pi-pico-LCD/lcd_display/include  # Include path for pi-pico-LCD
)

# Compiler options
add_compile_options(-Wall -Wno-format -Wno-unused-function -Wno-maybe-uninitialized)

# Add subdirectory for the pi-pico-LCD library
add_subdirectory(external/pi-pico-LCD/lcd_display)

# Link the LCD display library to the project
target_link_libraries(${PROJECT} 
    PUBLIC lcd_display  # Link the lcd_display library from the submodule
    PUBLIC pico_stdlib
    PUBLIC hardware_pwm
    PUBLIC hardware_adc
    PUBLIC hardware_i2c
    PUBLIC pico_rand
    PUBLIC hardware_pio
    PUBLIC dht
)

# Add subdirectories for other components
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
