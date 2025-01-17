#include <stdio.h>
#include "pico/stdlib.h"
#include "dht_handler.h"
#include "fan_control.h"
#include "buzzer.h"
#include "rgb_led.h"
// #include "../external/rpi-pico-i2c-display-lib/i2c-display-lib.h"
#include "../pico_dht/dht/include/dht.h"


#define BUTTON_PIN 3

bool is_manual_mode = false; 


const uint dht_pin = 15;
Fan fan1, fan2;

int main() {
    stdio_init_all();
    printf("Program started\n");
    buzzer_init();

    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);


    fan_init(&fan1, 10, 9, 8);
    fan_init(&fan2, 7, 6, 1);

    lcd_init(4, 5);  
    lcd_setAddr(0x27); 
    rgb_led_init(13, 12, 11);
    
    dht_handler_init(dht_pin);

    buzzer_beep(200);

    float temperature = 0.0f, humidity = 0.0f;

    while (true) {

        update_sensor_readings();

        toggle_mode(BUTTON_PIN);

        


        sleep_ms(500); 
    }

    return 0;
}
