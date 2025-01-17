#include <math.h>
#include <stdint.h>
#include "dht_handler.h"
#include "fan_control.h"
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "../external/rpi-pico-i2c-display-lib/i2c-display-lib.h"
#include "../pico_dht/dht/include/dht.h"
#include "rgb_led.h" 
#include "utils.h" 
#include "buzzer.h" 
 

extern float current_temperature;
extern bool is_mode_toggled;

#define MIN_DUTY_CYCLE 250

static dht_t dht_sensor;
extern Fan fan1;
extern Fan fan2;

static absolute_time_t last_buzzer_beep_time = 0;

static bool measurement_in_progress = false;
void adjust_fan_speed_by_temperature(float temperature);

void sensor_error(bool err);

void dht_handler_init(uint32_t pin) {    
    dht_init(&dht_sensor, DHT22, pio0, pin, true);
    dht_start_measurement(&dht_sensor);
}

bool read_temperature_and_humidity(float *temperature, float *humidity) {
    if (measurement_in_progress) {
        return false;
    }
    
    measurement_in_progress = true;
    dht_result_t result = dht_finish_measurement_blocking(&dht_sensor, humidity, temperature);
    measurement_in_progress = false;

    if (result != DHT_RESULT_OK) {
        return false;
    }

    return true;
}


//Funtion to reading temperature and setting the fan in the required direction.
// Tells the state of the fans and sounds buzzzer if temp exceeds a given limit
void adjust_fan_speed_by_temperature(float temperature) {
    static bool fans_running = false; 
    static bool buzzer_on = false; 


    //Fans are turned on at full speed (1023) when the temperature reaches or exceeds 25°C.
    if (temperature >= 25) {
        // fan_ramp_up(fan1.pwm_slice, fan1.pwm_channel);
        fan_set_speed(&fan1, 1023);
        // fan_set_speed(&fan2, 1023);
    
        fans_running = true;  
    } else if (temperature < 24 && fans_running) {
        // fan_ramp_down(fan1.pwm_slice, fan1.pwm_channel);
        stop_fan(&fan1);
        fans_running = false;  
    }
    //The second fan is engaged above 27°C and turned off at or below 26°C
    //The buzzer sounds if the temperature exceeds 27°C
    if (temperature > 27) {
        if (!buzzer_on || absolute_time_diff_us(last_buzzer_beep_time, get_absolute_time()) > 1000) { 
            buzzer_high_temp_alert(100); 
            last_buzzer_beep_time = get_absolute_time(); 
            buzzer_on = true;  
        }

        fan_set_speed(&fan1, 1023);
        // fan_ramp_up(fan2.pwm_slice, fan2.pwm_channel);
        fan_set_speed(&fan2, 1023);
    } else if (temperature <= 26 && buzzer_on) {
        stop_buzzer();
        // fan_ramp_down(fan2.pwm_slice, fan2.pwm_channel);
        stop_fan(&fan2);  
        buzzer_on = false; 
    }

    fan_set_direction(&fan1, true);  
    fan_set_direction(&fan2, true);  
}

bool first_reading_done = false;
bool read_dht_sensor() {
    float temperature, humidity;
    return read_temperature_and_humidity(&temperature, &humidity);
}

//Funtion to reading updating the 16x2 display LCD.
// Measurement of the sensor funcvtions are called and sounds buzzzer if there is a DHT timeout.
void update_sensor_readings() {
    
    static bool boot_message_shown = false;

    if (!boot_message_shown) {
        lcd_clear();
        lcd_setCursor(0, 0);
        lcd_print("Booting up...");
        sleep_ms(1000);
        lcd_clear();
        lcd_setCursor(1, 0);
        lcd_print("Welcome...");
        sleep_ms(2000); 
        boot_message_shown = true; 
    }

     if (!first_reading_done) {
        if (read_dht_sensor()) { 
            buzzer_beep(500); 
            first_reading_done = true;
        }
    }

    float temperature = 0.0f, humidity = 0.0f;
    absolute_time_t last_toggle_time = get_absolute_time();
    absolute_time_t last_manual_time = get_absolute_time();
    static absolute_time_t last_dht_time = 0;

    char line1[16];
    char line2[16];

    if (absolute_time_diff_us(last_dht_time, get_absolute_time()) < 5000) {
        return;  
    }

    last_dht_time = get_absolute_time();

    // Reading from the sensor
    
    dht_start_measurement(&dht_sensor);
    
    float temperature_c;
    dht_result_t result = dht_finish_measurement_blocking(&dht_sensor, &humidity, &temperature_c);

    // Checking if DHT functionality works well from reading from the sensor
    if (result == DHT_RESULT_OK) {
        printf("Temperature: %.1f°C, Humidity: %.1f%%\n", temperature_c, humidity);
        current_temperature = temperature_c;

        static float previous_temperature = -100.0f;
        static float previous_humidity = -100.0f;
        if (fabs(temperature_c - previous_temperature) > 0.3 || fabs(humidity - previous_humidity) > 3.0) {
            lcd_clear();
            snprintf(line1, sizeof(line1), "Temp: %.1fC", temperature_c);
            lcd_print(line1);

            snprintf(line2, sizeof(line2), "Hum: %.1f%%", humidity);
            lcd_setCursor(1,0);
            lcd_print(line2);

            adjust_fan_speed_by_temperature(temperature_c);
            fan_set_direction(&fan1, true);
            fan_set_direction(&fan2, true);

            rgb_led_update(temperature_c,is_mode_toggled);

            previous_temperature = temperature_c;
            previous_humidity = humidity;
        }

    } else if (result == DHT_RESULT_TIMEOUT) {
        sensor_error(true);  
        lcd_clear();
        lcd_setCursor(0, 0);
        lcd_print("DHT Timeout!");

        static absolute_time_t last_buzzer_time = 0;
        if (absolute_time_diff_us(last_buzzer_time, get_absolute_time()) > 10000) {
            buzzer_beep(100);  
            last_buzzer_time = get_absolute_time();
        }

            return;
    } else {
        
        sensor_error(true);
        lcd_clear();
        lcd_setCursor(0, 0);
        lcd_print("Checksum Error");
    } 
}

static bool is_manual_mode = false;  
static bool both_fans_on = false;  
static bool single_fan_on = false;

static const float threshold_temperature = 23.0;

// Switching modes
// Disables/ overides the fans temp based functionality and takes user input from button
// turning both/one fans off/on.. 
// calls buzzer and RGB lights ..
void toggle_mode(uint button_pin) {
    if (is_button_pressed(button_pin)) {
        
        is_manual_mode = !is_manual_mode;

        buzzer_beep(150); 
        lcd_clear();

        if (is_manual_mode) {
            rgb_set_white(); 

            lcd_setCursor(0, 0);
            lcd_print("Mode: Manual");

            stop_fan(&fan1);
            stop_fan(&fan2);
            both_fans_on = false;
            single_fan_on = false;

            lcd_setCursor(1, 0);
            lcd_print("Fans: Off");

            // longpress engages fan functionality as well mode exit
            if (is_button_pressed(button_pin)) {
                buzzer_beep(150); 
                lcd_clear();

                if (!both_fans_on && !single_fan_on) {
                    // fan_ramp_up(fan1.pwm_slice, fan1.pwm_channel);                    
                    fan_set_speed(&fan1, 1023);
                    // fan_ramp_up(fan2.pwm_slice, fan2.pwm_channel);
                    fan_set_speed(&fan2, 1023);
                    both_fans_on = true;
                    single_fan_on = false;

                    lcd_setCursor(0, 0);
                    lcd_print("Fans: On");
                } else if (both_fans_on) {
                    // fan_ramp_down(fan2.pwm_slice, fan2.pwm_channel);
                    stop_fan(&fan2);
                    single_fan_on = true;
                    both_fans_on = false;

                    lcd_setCursor(0, 0);
                    lcd_print("Fan 1: On");
                } else if (single_fan_on) {
                    // fan_ramp_down(fan1.pwm_slice, fan1.pwm_channel);
                    // fan_ramp_down(fan2.pwm_slice, fan2.pwm_channel);
                    stop_fan(&fan1);
                    stop_fan(&fan2);
                    single_fan_on = false;

                    lcd_setCursor(0, 0);
                    lcd_print("Fans: Off");
                }
            }
        } else {
            lcd_clear(); 
             
            lcd_setCursor(0, 0);
            lcd_print("Mode: Auto");

            if (current_temperature < threshold_temperature) {
                // fan_ramp_down(fan1.pwm_slice, fan1.pwm_channel);
                // fan_ramp_down(fan2.pwm_slice, fan2.pwm_channel);
                stop_fan(&fan1);
                stop_fan(&fan2);
                lcd_setCursor(1, 0);
                lcd_print("Fans: Off (Auto)");
            } else {
                adjust_fan_speed_by_temperature(current_temperature);
                rgb_led_update(current_temperature,is_mode_toggled);
                lcd_setCursor(1, 0);
                lcd_print("Fans: Auto");
            }
        }
    }
}

// Redudant code ../
// Updates the sensor reading after every 2 seconds
void dht_handler_loop() {
    
    while (1) {
        update_sensor_readings();  
        sleep_ms(2000);  
    }
}



