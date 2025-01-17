#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include <stdint.h>



extern float current_temperature; 

// header file for cross usage of button functionality

bool is_button_pressed(uint pin); 
void toggle_mode(uint button_pin); 


#endif // UTILS_H
