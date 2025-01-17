#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "fan_control.h"
#include <stdio.h>

// defined pins for the H bridge moteor driver
#define ENA_PIN 10
#define IN1_PIN 9
#define IN2_PIN 8
#define ENB_PIN 7
#define IN3_PIN 6
#define IN4_PIN 1

#define MIN_DUTY_CYCLE 250

//initializing primary and back up fan with PWM capabilities

void fan_init(Fan *fan, uint en_pin, uint in1_pin, uint in2_pin) {
    fan->en_pin = en_pin;
    fan->in1_pin = in1_pin;
    fan->in2_pin = in2_pin;
    
    gpio_init(fan->en_pin);
    gpio_set_function(fan->en_pin, GPIO_FUNC_PWM);
    gpio_init(fan->in1_pin);
    gpio_set_dir(fan->in1_pin, GPIO_OUT);
    gpio_init(fan->in2_pin);
    gpio_set_dir(fan->in2_pin, GPIO_OUT);
    
    fan->pwm_slice = pwm_gpio_to_slice_num(fan->en_pin);
    fan->pwm_channel = pwm_gpio_to_channel(fan->en_pin);
    
    //setting the wrap and clkdiv to ensure the fans operate within the usable range.
    pwm_set_wrap(fan->pwm_slice, 1023);
    pwm_set_clkdiv(fan->pwm_slice, 8.0f);
    pwm_set_chan_level(fan->pwm_slice, fan->pwm_channel, 0);
    pwm_set_enabled(fan->pwm_slice, true);
    printf("Fan initialized on EN pin: %d, PWM Slice: %d, Channel: %d\n", fan->en_pin, fan->pwm_slice, fan->pwm_channel);
}

//Setting the fan speed
//Using min duty cycle as the minimum and 1023 in the 10 bit resolution mode
void fan_set_speed(Fan *fan, uint16_t speed) {
    if (speed < MIN_DUTY_CYCLE) speed = MIN_DUTY_CYCLE; 
    if (speed > 1023) speed = 1023;
    // pwm_set_chan_level(fan->pwm_slice, fan->pwm_channel, 1023);
    // sleep_ms(10);
    pwm_set_chan_level(fan->pwm_slice, fan->pwm_channel, speed);
    printf("Setting fan speed to: %d (out of 1023)\n", speed);
}

// void fan_ramp_up(uint slice, uint channel) {
//     for (int duty = 512; duty <= 1023; duty++) {
//         pwm_set_chan_level(slice, channel, duty);
//         // sleep_ms(100); 
//     }
// }

// void fan_ramp_down(uint slice, uint channel) {
//     for (int duty = 1023; duty >= MIN_DUTY_CYCLE; duty--) {
//         pwm_set_chan_level(slice, channel, duty);
//         printf("Ramping down: Duty Cycle = %d\n", duty);
//         sleep_ms(100);
//     }
// }

//Setting the fan direction
void fan_set_direction(Fan *fan, bool forward) {
    if (forward) {
        gpio_put(fan->in1_pin, 1);
        gpio_put(fan->in2_pin, 0);
        
    } else {
        //Setting the fan direction in forward mode again beacause the fan spins in one direction
        gpio_put(fan->in1_pin, 1);
        gpio_put(fan->in2_pin, 0);
    }
}

//Stopping the fan after spinning
//Setting the PWM channel level to 0 effectively stops the fan
void stop_fan(Fan *fan) {
    pwm_set_chan_level(fan->pwm_slice, fan->pwm_channel, 0);
    printf("Fan stopped.\n");
}

