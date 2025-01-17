#ifndef FAN_CONTROL_H
#define FAN_CONTROL_H

#include "pico/stdlib.h"

// header file for cross usage of fan pins(enable Pins A and B) functionality
typedef struct {
    uint pwm_slice;
    uint pwm_channel;
    uint en_pin;
    uint in1_pin;
    uint in2_pin;
   
} Fan;

void fan_init(Fan *fan, uint en_pin, uint in1_pin, uint in2_pin);
void fan_set_speed(Fan *fan, uint16_t speed);
void fan_set_direction(Fan *fan, bool forward);
void fan_ramp_down(uint slice, uint channel);
void fan_ramp_up(uint slice, uint channel);

void stop_fan(Fan *fan);

#endif // FAN_CONTROL_H
