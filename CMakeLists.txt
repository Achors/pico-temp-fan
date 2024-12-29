cmake_minimum_required(VERSION 3.13)



# TODO: Set your project name here:
set(PROJECT pico-temp-fan)

# You do not need to modify the rest of this file!

if (NOT DEFINED ENV{PICO_SDK_PATH})
    message("PICO_SDK_PATH not set, looking for alternative paths")
    if (EXISTS $ENV{HOME}/Pico/pico-sdk)
        set(PICO_SDK_PATH $ENV{HOME}/Pico/pico-sdk)
    elseif(EXISTS $ENV{HOME}/pico/pico-sdk)
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

set(PICO_SDK_FETCH_FROM_GIT off)
include(pico_sdk_import.cmake)

project(${PROJECT})

pico_sdk_init()
include_directories(${CMAKE_SOURCE_DIR}/include)

add_compile_options(-Wall -Wno-format -Wno-unused-function -Wno-maybe-uninitialized)

# Add the submodule directory
add_subdirectory(pico-hd44780)

# Include necessary headers
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/pico-hd44780/include)

# File collection (ensure this is defined before add_executable)
file(GLOB SOURCES "*.h" "*.c")

# Create executable target
add_executable(${PROJECT} ${SOURCES})

# Link the submodule's library (assuming it's defined as such)
target_link_libraries(${PROJECT}
    pico_stdlib
    hardware_pwm
    hardware_adc
    pico_rand
    hardware_pio
    dht
    pico_hd44780  
)

# Additional subdirectory for dht
add_subdirectory(pico_dht/dht ${CMAKE_BINARY_DIR}/pico_dht_build)

pico_add_extra_outputs(${PROJECT})

pico_enable_stdio_usb(${PROJECT} 1)
pico_enable_stdio_uart(${PROJECT} 0)

if (${ENABLE_WIFI} AND PICO_CYW43_SUPPORTED)
    add_compile_definitions(WIFI)
    target_link_libraries(${PROJECT_NAME} pico_cyw43_arch_none)
endif ()
