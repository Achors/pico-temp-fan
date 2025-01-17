#include <stdio.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include "rgb_led.h"

// defined pins for the RGB LED use for visual update of temp range
static uint32_t red_pin, green_pin, blue_pin;

// Initializing the RGB LED to use in various functionalities of the set up.
void rgb_led_init(uint32_t red_pin_arg, uint32_t green_pin_arg, uint32_t blue_pin_arg) {
    red_pin = red_pin_arg;
    green_pin = green_pin_arg;
    blue_pin = blue_pin_arg;

    gpio_init(red_pin);
    gpio_set_dir(red_pin, GPIO_OUT);

    gpio_init(green_pin);
    gpio_set_dir(green_pin, GPIO_OUT);

    gpio_init(blue_pin);
    gpio_set_dir(blue_pin, GPIO_OUT);
}

//Setting the RGB to white when the system goes to manual mode
void rgb_set_white() {
    gpio_put(red_pin, 1);
    gpio_put(green_pin, 1);
    gpio_put(blue_pin, 1);
}

// Updating the RGB LED color depending on the mode of the system
// Updating the RGB LED color depending on the temperature of the system
void rgb_led_update(float temperature, bool is_mode_toggled) {
    static bool blink_done = false; 

    //RGB for flash/blink for mode change
    if (is_mode_toggled && !blink_done) {
        
        gpio_put(red_pin, 1);
        gpio_put(green_pin, 0);
        gpio_put(blue_pin, 0);
        sleep_ms(100);  
        gpio_put(red_pin, 0);
        gpio_put(green_pin, 0);
        gpio_put(blue_pin, 0);
        sleep_ms(100);

        blink_done = true;
    }

    //RGB for mode state
    //RGB update for temp level
   if (!is_mode_toggled || blink_done) {
    if (temperature < 22.0f) {
        gpio_put(red_pin, 0);
        gpio_put(green_pin, 0);
        gpio_put(blue_pin, 1);  
    } else if (temperature >= 22.0f && temperature < 25.0f) {
        gpio_put(red_pin, 0);
        gpio_put(green_pin, 1);
        gpio_put(blue_pin, 0);  
    } else if (temperature >= 25.0f && temperature <= 27.0f) {
        gpio_put(red_pin, 1);
        gpio_put(green_pin, 0.3);
        gpio_put(blue_pin, 0);  
    } else {
        gpio_put(red_pin, 1);
        gpio_put(green_pin, 0);
        gpio_put(blue_pin, 0);  
    }
    
    }
}

//RGB LED for DHT error 
void sensor_error(bool err) {
    if (err) {
        //RGB LED blink or flash red for error
        gpio_put(red_pin, 1);   
        gpio_put(green_pin, 0); 
        gpio_put(blue_pin, 0); 
        sleep_ms(100);          
        gpio_put(red_pin, 0);   
        gpio_put(green_pin, 0); 
        gpio_put(blue_pin, 0);  
    } else {
        
        gpio_put(red_pin, 0);   
        gpio_put(green_pin, 0); 
        gpio_put(blue_pin, 0);
    }
}
