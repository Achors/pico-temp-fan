#ifndef DHT_HANDLER_H
#define DHT_HANDLER_H


#include <stdbool.h>  
#include <stdint.h>   

// header file for cross usage of DHT sensor functionality

void dht_handler_init(uint32_t pin);


bool read_temperature_and_humidity(float *temperature, float *humidity);
void manual_mode_logic(uint32_t button_pin);

#endif // DHT_HANDLER_H
