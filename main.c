#include <stdio.h>
#include <dht.h>
#include <string.h>
#include <pico/stdlib.h>
#include <hardware/pwm.h>
#include <stdint.h>  // Include the stdint.h header for fixed-width integer types
#include "lcd.h"     // Include the LCD header

// DHT Sensor configuration
static const dht_model_t DHT_MODEL = DHT22;
static const uint DATA_PIN = 16;    // GPIO pin connected to DHT Data
static const uint RGB_RED_PIN = 13; // GPIO pin for Red
static const uint RGB_GREEN_PIN = 14; // GPIO pin for Green
static const uint RGB_BLUE_PIN = 15; // GPIO pin for Blue

// Define the GPIO pins for motor control
#define ENA_PIN 11  // PWM control for speed
#define IN1_PIN 10  // Direction control
#define IN2_PIN 9   // Direction control

// Convert Celsius to Fahrenheit
static float celsius_to_fahrenheit(float temperature) {
    return temperature * (9.0f / 5) + 32;
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
void set_motor_speed(uint8_t speed) {
    pwm_set_gpio_level(ENA_PIN, speed);
}

// Set the motor direction
void set_motor_direction(bool forward) {
    if (forward) {
        gpio_put(IN1_PIN, 1);
        gpio_put(IN2_PIN, 0);
    } else {
        gpio_put(IN1_PIN, 0);
        gpio_put(IN2_PIN, 1);
    }
}

// Set the RGB LED color
void set_rgb_color(uint8_t red, uint8_t green, uint8_t blue) {
    pwm_set_gpio_level(RGB_RED_PIN, red);
    pwm_set_gpio_level(RGB_GREEN_PIN, green);
    pwm_set_gpio_level(RGB_BLUE_PIN, blue);
}

int main() {
    stdio_init_all();
    puts("\nDHT22 Sensor, RGB LED, and Motor Control");

    dht_t dht;
    dht_init(&dht, DHT_MODEL, pio0, DATA_PIN, true);  // Initialize DHT22 sensor

    // Initialize motor control pins
    setup_pwm(ENA_PIN);
    gpio_init(IN1_PIN);
    gpio_set_dir(IN1_PIN, GPIO_OUT);
    gpio_init(IN2_PIN);
    gpio_set_dir(IN2_PIN, GPIO_OUT);

    // Initialize RGB LED pins for PWM
    setup_pwm(RGB_RED_PIN);
    setup_pwm(RGB_GREEN_PIN);
    setup_pwm(RGB_BLUE_PIN);

    // Initialize LCD
    lcd_init();

    // Set motor to forward direction initially
    set_motor_direction(true);

    while (true) {
        // Start the measurement
        dht_start_measurement(&dht);

        float humidity;
        float temperature_c;
        dht_result_t result = dht_finish_measurement_blocking(&dht, &humidity, &temperature_c);

        if (result == DHT_RESULT_OK) {
            printf("Temperature: %.1f C (%.1f F), Humidity: %.1f%%\n", 
                temperature_c, celsius_to_fahrenheit(temperature_c), humidity);

            // Update RGB LED color based on temperature
            if (temperature_c < 20.0) {
                set_rgb_color(0, 0, 255); // Blue for cold
            } else if (temperature_c > 25.0) {
                set_rgb_color(255, 0, 0); // Red for hot
            } else {
                set_rgb_color(0, 255, 0); // Green for normal
            }

            // Motor speed control based on temperature
            uint8_t motor_duty_cycle;
            if (temperature_c < 20.0) {
                motor_duty_cycle = 0;  // Motor off
            } else if (temperature_c > 30.0) {
                motor_duty_cycle = 255;  // Max speed
            } else {
                motor_duty_cycle = (uint8_t)(255 * (temperature_c - 20.0) / 10.0);  // Linearly increase speed
            }

            // Set the motor speed based on the calculated duty cycle
            set_motor_speed(motor_duty_cycle);

            // Display temperature and humidity on the LCD
            lcd_clear();  
            lcd_set_cursor(0, 0);  
            char line1[16];
            snprintf(line1, sizeof(line1), "Temp: %.1fC", temperature_c);
            lcd_send_data((uint8_t*)line1);

            lcd_set_cursor(1, 0);  
            char line2[16];
            snprintf(line2, sizeof(line2), "Hum: %.1f%%", humidity);
            lcd_send_data((uint8_t*)line2);

        } else if (result == DHT_RESULT_TIMEOUT) {
            puts("DHT sensor not responding. Please check your wiring.");
            set_rgb_color(0, 0, 0);  // Turn off the LED on timeout
            lcd_clear();         
            lcd_set_cursor(0, 0);
            lcd_send_data((uint8_t*)"DHT Timeout!");
        } else {
            puts("Bad checksum");
            set_rgb_color(0, 0, 0);  // Turn off the LED on checksum error
            lcd_clear();         
            lcd_set_cursor(0, 0);
            lcd_send_data((uint8_t*)"Checksum Error");
        }

        sleep_ms(2000);  // Wait 2 seconds before next reading
    }

    return 0;
}
