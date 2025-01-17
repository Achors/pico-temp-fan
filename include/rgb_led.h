#ifndef RGB_LED_H
#define RGB_LED_H


// header file for cross usage of RGB LED functionality
void rgb_led_init(uint32_t red_pin, uint32_t green_pin, uint32_t blue_pin);
void rgb_set_white();
void sensor_error(bool err);


void rgb_led_update(float temperature, bool is_mode_toggled);

#endif // RGB_LED_H