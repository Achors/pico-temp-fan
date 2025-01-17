#ifndef BUZZER_H
#define BUZZER_H

#include <stdint.h>


// header file for cross usage of buzzer functionality

void buzzer_init();

void buzzer_beep();

void buzzer_high_temp_alert();

void buzzer_dht_timeout_alert();

void stop_buzzer();

#endif // BUZZER_H
