// #include <stdio.h>
// #include <stdint.h>
// #include "pico/stdlib.h"
// #include "hardware/pwm.h"

// // Define GPIO pins for Fan 1
// #define ENA_PIN 10
// #define IN1_PIN 9
// #define IN2_PIN 8

// // Define GPIO pins for Fan 2
// #define ENB_PIN 7   // PWM control for speed (Fan 2)
// #define IN3_PIN 6   // Direction control (Fan 2)
// #define IN4_PIN 1   // Direction control (Fan 2)

// typedef struct {
//     uint pwm_slice;
//     uint pwm_channel;
//     uint en_pin;
//     uint in1_pin;
//     uint in2_pin;
// } Fan;

// void fan_init(Fan *fan, uint en_pin, uint in1_pin, uint in2_pin) {
//     fan->en_pin = en_pin;
//     fan->in1_pin = in1_pin;
//     fan->in2_pin = in2_pin;

//     // GPIO setup
//     gpio_init(fan->en_pin);
//     gpio_set_function(fan->en_pin, GPIO_FUNC_PWM);  // Set ENA pin for PWM output
//     gpio_init(fan->in1_pin);
//     gpio_set_dir(fan->in1_pin, GPIO_OUT);  // Set direction control pins
//     gpio_init(fan->in2_pin);
//     gpio_set_dir(fan->in2_pin, GPIO_OUT);  // Set direction control pins

//     // PWM setup
//     fan->pwm_slice = pwm_gpio_to_slice_num(fan->en_pin);
//     fan->pwm_channel = pwm_gpio_to_channel(fan->en_pin);
//     pwm_set_wrap(fan->pwm_slice, 1023);  // Set PWM wrap for 10-bit resolution
//     pwm_set_clkdiv(fan->pwm_slice, 512.0f);  // Adjust clock divider for proper frequency

//     pwm_set_chan_level(fan->pwm_slice, fan->pwm_channel, 0);  // Initialize PWM to 0
//     pwm_set_enabled(fan->pwm_slice, true);  // Enable PWM for the fan

//     printf("Fan initialized on EN pin: %d, PWM Slice: %d, Channel: %d\n",
//            fan->en_pin, fan->pwm_slice, fan->pwm_channel);
// }

// void fan_set_speed(Fan *fan, uint16_t speed) {
//     if (speed > 1023) speed = 1023;  // Ensure the speed is within the valid range
//     pwm_set_chan_level(fan->pwm_slice, fan->pwm_channel, speed);  // Set PWM duty cycle
//     printf("Setting speed to: %d (out of 1023)\n", speed);
// }

// void fan_set_direction(Fan *fan, bool forward) {
//     // Set direction for the fan
//     if (forward) {
//         gpio_put(fan->in1_pin, 0);  // Set IN1 low
//         gpio_put(fan->in2_pin, 1);  // Set IN2 high
//     } else {
//         gpio_put(fan->in1_pin, 1);  // Set IN1 high
//         gpio_put(fan->in2_pin, 0);  // Set IN2 low
//     }
// }

// void stop_fan(Fan *fan) {
//     // Stop the fan by setting the PWM duty cycle to 0
//     pwm_set_chan_level(fan->pwm_slice, fan->pwm_channel, 0);
//     printf("Fan stopped.\n");
// }

// int main() {
//     stdio_init_all();

//     // Initialize the fans
//     Fan fan1, fan2;
//     fan_init(&fan1, ENA_PIN, IN1_PIN, IN2_PIN);
//     fan_init(&fan2, ENB_PIN, IN3_PIN, IN4_PIN);

//     while (true) {
//         // Fan 1 at 50% speed
//         fan_set_direction(&fan1, true);  // Set fan 1 to rotate forward
//         fan_set_speed(&fan1, 1023);  // Set 50% duty cycle (out of 1023)
//         sleep_ms(5000);

//         // Fan 2 at 50% speed
//         fan_set_direction(&fan2, true);  // Set fan 2 to rotate forward
//         fan_set_speed(&fan2, 1023);  // Set 50% duty cycle (out of 1023)
//         sleep_ms(10000);

//         // Stop both fans
//         stop_fan(&fan1);
//         stop_fan(&fan2);
//         sleep_ms(2000);  // Wait before restarting the cycle
//     }
// }
