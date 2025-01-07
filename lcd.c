#include "lcd.h"
#include <pico/stdlib.h>
#include <hardware/i2c.h>
#include <stdint.h>

// Define I²C pins and address
#define I2C_SDA_PIN 4  // SDA pin
#define I2C_SCL_PIN 5  // SCL pin
#define LCD_I2C_ADDR 0x27  // Default I²C address for most 4-pin LCD modules

// Function prototypes
static void lcd_send(uint8_t value, uint8_t mode);
static void lcd_pulse_enable(uint8_t data);

// Initialize the LCD via I²C
void lcd_init() {
    // Initialize I²C
    i2c_init(i2c0, 100 * 1000);  // 100 kHz I²C
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);

    sleep_ms(50);  // Wait for LCD to power up

    // LCD initialization sequence
    lcd_send(0x03, 0);  // Reset sequence 1
    sleep_ms(5);
    lcd_send(0x03, 0);  // Reset sequence 2
    sleep_us(150);
    lcd_send(0x03, 0);  // Reset sequence 3
    sleep_ms(5);
    lcd_send(0x02, 0);  // Set to 4-bit mode

    // Function Set: 2 Line, 4-bit, 5x8 dots
    lcd_send_command(0x28);

    // Display ON, Cursor OFF
    lcd_send_command(0x0C);

    // Entry Mode Set: Increment cursor, no shift
    lcd_send_command(0x06);

    // Clear display
    lcd_clear();
}

// Send a command to the LCD
void lcd_send_command(uint8_t command) {
    lcd_send(command, 0);  // Mode 0 for commands
}

// Send data to the LCD
void lcd_send_data(uint8_t data) {
    lcd_send(data, 1);  // Mode 1 for data
}

// Move the cursor to a specific position
void lcd_set_cursor(uint8_t row, uint8_t col) {
    uint8_t address = (row == 0) ? (0x80 + col) : (0xC0 + col);
    lcd_send_command(address);
}

// Clear the LCD display
void lcd_clear() {
    lcd_send_command(0x01);  // Clear display
    sleep_ms(2);  // Allow time for the command to complete
}

// Helper function to send data to the LCD
static void lcd_send(uint8_t value, uint8_t mode) {
    uint8_t high_nibble = (value & 0xF0) | (mode ? 0x01 : 0x00);
    uint8_t low_nibble = ((value << 4) & 0xF0) | (mode ? 0x01 : 0x00);

    lcd_pulse_enable(high_nibble);
    lcd_pulse_enable(low_nibble);
}

// Generate an enable pulse for the LCD
static void lcd_pulse_enable(uint8_t data) {
    uint8_t data_with_enable = data | 0x04;  // Enable bit set
    uint8_t data_without_enable = data & ~0x04;  // Enable bit cleared

    i2c_write_blocking(i2c0, LCD_I2C_ADDR, &data_with_enable, 1, false);
    sleep_us(1);  // Short pulse
    i2c_write_blocking(i2c0, LCD_I2C_ADDR, &data_without_enable, 1, false);
    sleep_us(50);  // Short delay for LCD to process
}
