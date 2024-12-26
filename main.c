#include <stdio.h>
#include <dht.h>
#include <pico/stdlib.h>
#include <hardware/pwm.h>
#include "RPI_PICO_I2C_LCD.h"

// DHT Sensor configuration
static const dht_model_t DHT_MODEL = DHT22;
static const uint DATA_PIN = 16;    // GPIO pin connected to DHT Data
static const uint RGB_RED_PIN = 13; // GPIO pin for Red
static const uint RGB_GREEN_PIN = 14; // GPIO pin for Green
static const uint RGB_BLUE_PIN = 15; // GPIO pin for Blue

// I2C display configuration
#define I2C_PORT i2c0
#define LCD_ADDR 0x27  // Default address for 16x2 I2C LCD (you may need to change this)
#define SDA_PIN 18
#define SCL_PIN 19

// Convert Celsius to Fahrenheit
static float celsius_to_fahrenheit(float temperature) {
    return temperature * (9.0f / 5) + 32;
}

// Initialize PWM for RGB LED
void setup_pwm(uint pin) {
    gpio_set_function(pin, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(pin);
    pwm_set_wrap(slice_num, 255);  // Set PWM range to 0-255 for 8-bit color
    pwm_set_gpio_level(pin, 0);    // Set initial duty cycle to 0
    pwm_set_enabled(slice_num, true);
}

// Set the RGB LED color
void set_rgb_color(uint8_t red, uint8_t green, uint8_t blue) {
    pwm_set_gpio_level(RGB_RED_PIN, red);
    pwm_set_gpio_level(RGB_GREEN_PIN, green);
    pwm_set_gpio_level(RGB_BLUE_PIN, blue);
}

// Initialize I2C LCD
lcd_t lcd;

void setup_i2c_lcd() {
    i2c_init(I2C_PORT, 400000);  // Initialize I2C with 400kHz clock speed
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);  // Set SDA pin for I2C
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);  // Set SCL pin for I2C
    gpio_pull_up(SDA_PIN);  // Enable pull-up resistors
    gpio_pull_up(SCL_PIN);  // Enable pull-up resistors

    lcd_init(&lcd, LCD_ADDR, &I2C_PORT);  // Initialize the LCD with address 0x27
    lcd_clear(&lcd);  // Clear the LCD display
}

int main() {
    stdio_init_all();
    puts("\nDHT22 Sensor, RGB LED and LCD Display Control");

    dht_t dht;
    dht_init(&dht, DHT_MODEL, pio0, DATA_PIN, true);  // Initialize DHT22 sensor

    // Initialize RGB LED pins for PWM
    setup_pwm(RGB_RED_PIN);
    setup_pwm(RGB_GREEN_PIN);
    setup_pwm(RGB_BLUE_PIN);

    // Initialize I2C LCD display
    setup_i2c_lcd();

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
                // Blue for cold (below 20°C)
                set_rgb_color(0, 0, 255);
            } else if (temperature_c > 25.0) {
                // Red for hot (above 25°C)
                set_rgb_color(255, 0, 0);
            } else {
                // Green for normal (between 20°C and 25°C)
                set_rgb_color(0, 255, 0);
            }

            // Display temperature and humidity on the LCD
            char buffer[32];
            snprintf(buffer, sizeof(buffer), "T: %.1fC H: %.1f%%", temperature_c, humidity);
            lcd_set_cursor(&lcd, 0, 0);
            lcd_print(&lcd, buffer);

        } else if (result == DHT_RESULT_TIMEOUT) {
            puts("DHT sensor not responding. Please check your wiring.");
            set_rgb_color(0, 0, 0);  // Turn off the LED on timeout
            lcd_clear(&lcd);  // Clear the display
        } else {
            assert(result == DHT_RESULT_BAD_CHECKSUM);
            puts("Bad checksum");
            set_rgb_color(0, 0, 0);  // Turn off the LED on checksum error
            lcd_clear(&lcd);  // Clear the display
        }

        sleep_ms(2000);  // Wait 2 seconds before next reading
    }

    return 0;
}
