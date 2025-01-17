#include "pico/stdlib.h"
#include "buzzer.h"


// defined buzzer pin for Sound alarming
static const uint BUZZER_PIN = 0;

// inittialzed the buzzer functionality
void buzzer_init() {
    gpio_init(BUZZER_PIN);
    gpio_set_dir(BUZZER_PIN, GPIO_OUT);
    gpio_put(BUZZER_PIN, 0);
}

// Beep for button press
// Beep for mode change
void buzzer_beep() {
    gpio_put(BUZZER_PIN, 1);
    sleep_ms(100);
    gpio_put(BUZZER_PIN, 0);
}

// ALert for extreme temp conditions
void buzzer_high_temp_alert() {
    gpio_put(BUZZER_PIN, 1);
    sleep_ms(100);
    gpio_put(BUZZER_PIN, 0);
}

// Stopping buzzer sounds
void stop_buzzer() {
    gpio_put(BUZZER_PIN, 0);
}


// ALert for dht error or issues
void buzzer_dht_timeout_alert() {
    gpio_put(BUZZER_PIN, 1);
    sleep_ms(100);
    gpio_put(BUZZER_PIN, 0);
    sleep_ms(100);
}
