#ifndef LCD_H
#define LCD_H

#include <stdint.h>

// Initialize the I²C LCD
void lcd_init();

// Send a command to the LCD
void lcd_send_command(uint8_t command);

// Send data (e.g., characters) to the LCD
void lcd_send_data(uint8_t data);

// Move the cursor to a specific position
void lcd_set_cursor(uint8_t row, uint8_t col);

// Clear the LCD display
void lcd_clear();

#endif // LCD_H
