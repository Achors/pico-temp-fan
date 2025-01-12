#include <stdio.h>
#include "dht.h"
#include <math.h>
#include <string.h>
#include <pico/stdlib.h>
#include <hardware/pwm.h>
#include <hardware/adc.h>
#include <stdint.h>  // Include the stdint.h header for fixed-width integer types
#include "i2c-display-lib.h" //LCD Display 

// DHT Sensor configuration
static const dht_model_t DHT_MODEL = DHT22;
static const uint DATA_PIN = 15;    // GPIO pin connected to DHT Data
//RGB LED configuration
static const uint RGB_RED_PIN = 13; // GPIO pin for Red
static const uint RGB_GREEN_PIN = 12; // GPIO pin for Green
static const uint RGB_BLUE_PIN = 11; // GPIO pin for Blue


//Last Speed
static uint8_t last_set_speed = 0;

// Define the GPIO pins for motor control
#define ENA_PIN 10  // PWM control for speed (Fan 1)
#define IN1_PIN 9  // Direction control (Fan 1)
#define IN2_PIN 8   // Direction control (Fan 1)
#define ENB_PIN 7  // PWM control for speed (Fan 2)
#define IN3_PIN 6  // Direction control (Fan 2)
#define IN4_PIN 1   // Direction control (Fan 2)

// Minimum duty cycle for smooth fan operation
#define MIN_DUTY_CYCLE 64  // ~25%

#define MODE_BUTTON_PIN 0  // GPIO pin for the mode toggle button
#define MANUAL_BUTTON_PIN 3      // Second button for manual control

// PID Parameters
#define Kp 2.0f  // Proportional gain
#define Ki 0.1f  // Integral gain
#define Kd 0.5f  // Derivative gain

static float integral = 0.0f;  // Accumulated error for the integral term
static float prev_error = 0.0f;  // Previous error for the derivative term

// Fan operation mode
bool tandem_mode = false;  // Start in single fan mode

// Add variables for RGB flashing and mode toggling
bool manual_mode = false;       // Default to automatic mode
bool mode_toggle = false;       // Flag for mode change
absolute_time_t last_mode_toggle_time;
absolute_time_t last_manual_time;

float target_temperature = 23.0f;

void set_rgb_color(uint8_t red, uint8_t green, uint8_t blue);
float compute_pid(float setpoint, float measured);



// Convert Celsius to Fahrenheit
static float celsius_to_fahrenheit(float temperature) {
    return temperature * (9.0f / 5) + 32;
}

// Initialize the ADC for potentiometer input
void setup_adc(uint pin) {
    adc_init();
    adc_gpio_init(pin);  // Initialize the GPIO pin for ADC
    adc_select_input(0); // Select ADC channel 0 (assuming potentiometer is on GPIO26)
}

// Map ADC value (0-4095) to PWM range (0-255)
uint8_t map_adc_to_pwm(uint16_t adc_value) {
    // Remap ADC value from [25, 50] to [0, 255]
    if (adc_value < 25) {
        adc_value = 25;  // Clamping to the lower bound
    } else if (adc_value > 50) {
        adc_value = 50;  // Clamping to the upper bound
    }

    // Remap the value
    uint8_t mapped_pwm = (uint8_t)((adc_value - 25) * 255 / (50 - 25));
    return mapped_pwm;
}

// Initialize PWM for Motor
void setup_pwm(uint pin) {
    gpio_set_function(pin, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(pin);
    pwm_set_wrap(slice_num, 255);  // Set PWM range to 0-255 for 8-bit control
    pwm_set_gpio_level(pin, 0);    // Set initial duty cycle to 0 (motor off)
    pwm_set_enabled(slice_num, true);
}

// Set the PWM duty cycle for motor control
void set_motor_speed(uint pin, uint8_t speed) {
    uint slice_num = pwm_gpio_to_slice_num(pin);
    pwm_set_gpio_level(pin, speed);
}

void toggle_mode() {
    if (tandem_mode) {
        set_rgb_color(255, 255, 0);  // Flash yellow for tandem mode
    } else {
        set_rgb_color(255, 0, 255);  // Flash pink for single mode
    }
    sleep_ms(300);                  // Flash duration
    set_rgb_color(0, 0, 0);        // Turn off LED after flash
    sleep_ms(100);                  // Wait before toggling mode

    // Toggle between tandem and single mode
    tandem_mode = !tandem_mode;
    if (tandem_mode) {
        printf("Mode changed: Tandem\n");
    } else {
        printf("Mode changed: Single\n");
    }
}


// void toggle_mode() {
//     static int mode = 0; // 0: Manual, 1: Automatic, 2: Tandem
//     mode = (mode + 1) % 3;

//     switch (mode) {
//         case 0:
//             manual_mode = true;
//             tandem_mode = false;
//             set_rgb_color(0, 0, 255); // Flash Blue for manual mode
//             break;
//         case 1:
//             manual_mode = false;
//             tandem_mode = false;
//             set_rgb_color(0, 255, 0); // Flash Green for automatic mode
//             break;
//         case 2:
//             manual_mode = false;
//             tandem_mode = true;
//             set_rgb_color(255, 255, 0); // Flash Yellow for tandem mode
//             break;
//     }
//     sleep_ms(300);
//     set_rgb_color(0, 0, 0); // Turn off LED after flash
// }

// Set the motor direction
void set_motor_direction(uint pin1, uint pin2, bool forward) {
    if (forward) {
        gpio_put(pin1, 1);
        gpio_put(pin2, 0);
    } else {
        gpio_put(pin1, 0);
        gpio_put(pin2, 1);
    }
}

// Button debounce logic
bool is_button_pressed(uint button_pin) {
    static bool last_state = false;
    static absolute_time_t last_time = {0};
    bool current_state = gpio_get(button_pin);
    absolute_time_t now = get_absolute_time();

    if (current_state && !last_state && absolute_time_diff_us(last_time, now) > 300000) {
        last_time = now;  // Update debounce time
        last_state = true;
        return true;
    }

    last_state = current_state;
    return false;
}

// PID Controller Function
float compute_pid(float setpoint, float measured) {
    float error = setpoint - measured;
    integral += error;
    float derivative = error - prev_error;
    prev_error = error;

    float output = (Kp * error) + (Ki * integral) + (Kd * derivative);

    // Clamp the output to the range [4, 200]
    if (output < 4.0f) {
        output = 4.0f;
    } else if (output > 200.0f) {
        output = 200.0f;
    }

    return output;
}


void handle_fan_speed(float temperature_c, uint8_t manual_speed) {
    if (manual_mode) {
        // Manual mode: directly set motor speed
        set_motor_direction(IN1_PIN, IN2_PIN, true); // Set motor direction (forward)
        set_motor_speed(ENA_PIN, manual_speed);  // Set Fan 1 speed based on manual input

        if (tandem_mode) {
            // Fan 2 at 80% speed in tandem mode
            set_motor_speed(ENB_PIN, (uint8_t)(manual_speed * 0.8));  // Fan 2 at 80% speed
            set_motor_direction(IN3_PIN, IN4_PIN, true); // Set second motor direction (forward)
        } else {
            // Ensure Fan 2 is off in single mode
            set_motor_speed(ENB_PIN, 0);
        }

    } else {
        // PID-based automatic control when not in manual mode
        float pid_output = compute_pid(target_temperature, temperature_c);

        // Ensure PID output falls within valid range
        uint8_t duty_cycle = (uint8_t)(pid_output > 255 ? 255 : (pid_output < MIN_DUTY_CYCLE ? MIN_DUTY_CYCLE : pid_output));

        // Start fan with initial burst if duty_cycle > 0 and fan is stopped
        if (duty_cycle > 0 && last_set_speed == 0) {
            set_motor_direction(IN1_PIN, IN2_PIN, true);  // Set motor direction (forward)
            set_motor_speed(ENA_PIN, 128);  // Initial burst to start the fan
            sleep_ms(100);                 // Wait for fan to start
        }

        // Set fan speed based on PID-adjusted duty cycle
        set_motor_speed(ENA_PIN, duty_cycle);
        last_set_speed = duty_cycle;  // Update the last set speed

        // Tandem mode: adjust second fan speed
        if (tandem_mode) {
            uint8_t tandem_speed = (uint8_t)(duty_cycle * 0.8);  // Fan 2 at 80% speed
            set_motor_speed(ENB_PIN, tandem_speed);
            set_motor_direction(IN3_PIN, IN4_PIN, true);  // Set second motor direction (forward)
        } else {
            // Ensure Fan 2 is off in single mode
            set_motor_speed(ENB_PIN, 0);
        }

        // Debugging logs for automatic mode
        printf("Auto Mode - Temp: %.2f°C, Target: %.2f°C, PID: %.2f, Fan Speed: %d\n",
               temperature_c, target_temperature, pid_output, duty_cycle);
    }
}





// Set the RGB LED color
void set_rgb_color(uint8_t red, uint8_t green, uint8_t blue) {
    pwm_set_gpio_level(RGB_RED_PIN, red);
    pwm_set_gpio_level(RGB_GREEN_PIN, green);
    pwm_set_gpio_level(RGB_BLUE_PIN, blue);
}

void rgb_flash_white() {
    set_rgb_color(255, 255, 255);  // Flash white
    sleep_ms(300);                  // Flash duration
    set_rgb_color(0, 0, 0);        // Turn off LED after flash
}



void update_rgb_for_temperature(float temperature) {
    if (temperature < 23.0f) {
        set_rgb_color(0, 255, 0);  // Blue for cold
    } else if (temperature >= 23.0f || temperature <= 25.0f) {
        set_rgb_color(0, 0, 255);  // Green for normal
        
    } else {
        set_rgb_color(255, 0, 0);  // Red for hot
       
    }
}

int main() {
    stdio_init_all();
    lcd_init(4, 5);
    puts("\nDHT22 Sensor, RGB LED, and Motor Control with PID");

    setup_adc(26);

    dht_t dht;
    dht_init(&dht, DHT_MODEL, pio0, DATA_PIN, true);  // Initialize DHT22 sensor

    // Initialize motor control pins
    setup_pwm(ENA_PIN);
    gpio_init(IN1_PIN);
    gpio_set_dir(IN1_PIN, GPIO_OUT);
    gpio_init(IN2_PIN);
    gpio_set_dir(IN2_PIN, GPIO_OUT);

    setup_pwm(ENB_PIN);
    gpio_init(IN3_PIN);
    gpio_set_dir(IN3_PIN, GPIO_OUT);
    gpio_init(IN4_PIN);
    gpio_set_dir(IN4_PIN, GPIO_OUT);

    // Initialize RGB LED pins for PWM
    setup_pwm(RGB_RED_PIN);
    setup_pwm(RGB_GREEN_PIN);
    setup_pwm(RGB_BLUE_PIN);

    // Initialize button pins
    gpio_init(MODE_BUTTON_PIN);
    gpio_set_dir(MODE_BUTTON_PIN, GPIO_IN);
    gpio_pull_up(MODE_BUTTON_PIN);

    gpio_init(MANUAL_BUTTON_PIN);
    gpio_set_dir(MANUAL_BUTTON_PIN, GPIO_IN);
    gpio_pull_up(MANUAL_BUTTON_PIN);

    // Initialize state variables
    bool tandem_mode = false;       // Default to single fan mode
    absolute_time_t last_toggle_time = get_absolute_time();
    absolute_time_t last_manual_time = get_absolute_time();

    // Declare line1 and line2 for LCD output
    char line1[16];
    char line2[16];

    while (true) {
        // Handle button for mode toggling (debounced)
        if (gpio_get(MODE_BUTTON_PIN) == 0 && 
            absolute_time_diff_us(last_toggle_time, get_absolute_time()) > 300000) {
            toggle_mode(); // Flash RGB and toggle mode
            last_toggle_time = get_absolute_time();
        }

        // Handle button for manual control (debounced)
        if (gpio_get(MANUAL_BUTTON_PIN) == 0 && 
            absolute_time_diff_us(last_manual_time, get_absolute_time()) > 300000) {
            if (!manual_mode) {
                manual_mode = true;  // Enter manual mode
                rgb_flash_white();   // Flash white to indicate manual mode activation
                printf("Manual mode enabled.\n");
                lcd_clear();  
                lcd_setCursor(0,0);
                lcd_print("Manual mode");
            } else {
                manual_mode = false;  // Exit manual mode
                rgb_flash_white();    // Flash white to indicate exit
                printf("Returning to automatic mode.\n");
                lcd_clear();  
                lcd_setCursor(0,0);
                lcd_print("Auto mode");
            }
            last_manual_time = get_absolute_time();  // Update the debounce time
        }

        if (manual_mode) {
            // Read potentiometer value
            uint16_t pot_value = adc_read();  // Read ADC value
            uint8_t manual_fan_speed = map_adc_to_pwm(pot_value);  // Map ADC value to PWM range
            if (manual_fan_speed < MIN_DUTY_CYCLE) {
                manual_fan_speed = MIN_DUTY_CYCLE;  // Ensure the manual speed is at least the minimum duty cycle
            }

            // Debug print for manual fan speed
            printf("Manual fan speed: %d (ADC: %d)\n", manual_fan_speed, pot_value);

            // Display manual fan speed on LCD
            lcd_clear();
            snprintf(line1, sizeof(line1), "Fan Speed: %d", manual_fan_speed);
            lcd_print(line1);

            // Adjust fan speed using potentiometer in manual mode
            handle_fan_speed(target_temperature, manual_fan_speed);  // Call the handle_fan_speed function
        } else {
            // DHT sensor reading and automatic fan control
            static absolute_time_t last_dht_time = 0;
            if (absolute_time_diff_us(last_dht_time, get_absolute_time()) < 250000) {
                continue;
            }
            last_dht_time = get_absolute_time();
            dht_start_measurement(&dht);
            float humidity;
            float temperature_c;
            dht_result_t result = dht_finish_measurement_blocking(&dht, &humidity, &temperature_c);  // Correct function call

            if (result == DHT_RESULT_OK) {
                printf("Temperature: %.1f°C, Humidity: %.1f%%\n", temperature_c, humidity);

                // Avoid excessive updates
                static float previous_temperature = -100.0f;
                static float previous_humidity = -100.0f;
                if (fabs(temperature_c - previous_temperature) > 0.5 || fabs(humidity - previous_humidity) > 5.0) {
                    lcd_clear();
                    snprintf(line1, sizeof(line1), "Temp: %.1fC", temperature_c);
                    lcd_print(line1);

                    snprintf(line2, sizeof(line2), "Hum: %.1f%%", humidity);
                    lcd_setCursor(1,0);
                    lcd_print(line2);

                    previous_temperature = temperature_c;
                    previous_humidity = humidity;
                }

                // RGB LED logic based on temperature
                if (temperature_c < 23.0f) {
                    set_rgb_color(0, 255, 0);  // Blue for cold
                } else if (temperature_c >= 23.0f && temperature_c <= 25.0f) {
                    set_rgb_color(0, 0, 255);  // Green for normal
                } else {
                    set_rgb_color(255, 0, 0);  // Red for hot
                }

                // Control fan speed based on temperature and PID
                float pid_output = compute_pid(target_temperature, temperature_c);
                printf("PID Output: %.2f\n", pid_output);

                // Apply minimum fan speed and temperature threshold
                if (temperature_c > 23.0f) {
                    uint8_t fan_speed = (uint8_t)pid_output;
                    if (fan_speed < MIN_DUTY_CYCLE) {
                        fan_speed = MIN_DUTY_CYCLE;  // Ensure the fan speed is not below the minimum duty cycle
                    } else if (fan_speed > 255) {
                        fan_speed = 255;  // Clamp the fan speed to the maximum value
                    }
                    handle_fan_speed(temperature_c, fan_speed);  // Apply fan speed control
                } else {
                    handle_fan_speed(temperature_c, 0);  // Stop fan below 23°C
                }
            } else if (result == DHT_RESULT_TIMEOUT) {
                puts("DHT sensor not responding. Please check your wiring.");
                set_rgb_color(255, 0, 0);  // Red for timeout
                sleep_ms(100);
                set_rgb_color(0, 0, 0);  // Turn off the LED on timeout
                lcd_clear();         
                lcd_setCursor(0,0);
                lcd_print("DHT Timeout!");
            } else {
                puts("Bad checksum");
                set_rgb_color(0, 0, 0);  // Turn off the LED on checksum error
                lcd_clear();         
                lcd_setCursor(0,0);
                lcd_print("Checksum Error");
            }
        }

        // Delay to allow for periodic updates
        sleep_ms(100);
    }

    return 0;
}
