#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "utils.h"
#include "rgb_led.h"
#include "pico/stdlib.h"

// button debounce time..time wait before next press signal
#define DEBOUNCE_TIME_MS 50

float current_temperature = 0.0f;
bool is_mode_toggled = false; 

// Function for button double functionality
bool is_button_pressed(uint pin) {
    static absolute_time_t last_press_time = 0;

    //  Function for button long press functionality
    if (absolute_time_diff_us(last_press_time, get_absolute_time()) < DEBOUNCE_TIME_MS * 1000) {
        return false; 
    }

    if (gpio_get(pin) == 0) { 
        last_press_time = get_absolute_time();
        return true;
    }
    return false;
}


