// -------------------------------------------------- //
// This file is autogenerated by pioasm; do not edit! //
// -------------------------------------------------- //

#pragma once

#if !PICO_NO_HARDWARE
#include "hardware/pio.h"
#endif

// --- //
// dht //
// --- //

#define dht_wrap_target 0
#define dht_wrap 16
#define dht_pio_version 0

#define dht_start_signal_clocks_per_loop 1
#define dht_pulse_measurement_clocks_per_loop 2

static const uint16_t dht_program_instructions[] = {
            //     .wrap_target
    0x0080, //  0: jmp    y--, 0                     
    0xe080, //  1: set    pindirs, 0                 
    0x00c2, //  2: jmp    pin, 2                     
    0x00c5, //  3: jmp    pin, 5                     
    0x0003, //  4: jmp    3                          
    0x00c5, //  5: jmp    pin, 5                     
    0x00c8, //  6: jmp    pin, 8                     
    0x0006, //  7: jmp    6                          
    0xa047, //  8: mov    y, osr                     
    0x00cb, //  9: jmp    pin, 11                    
    0x000f, // 10: jmp    15                         
    0x0089, // 11: jmp    y--, 9                     
    0xe021, // 12: set    x, 1                       
    0x4021, // 13: in     x, 1                       
    0x0005, // 14: jmp    5                          
    0x4061, // 15: in     null, 1                    
    0x0006, // 16: jmp    6                          
            //     .wrap
};

#if !PICO_NO_HARDWARE
static const struct pio_program dht_program = {
    .instructions = dht_program_instructions,
    .length = 17,
    .origin = -1,
    .pio_version = 0,
#if PICO_PIO_VERSION > 0
    .used_gpio_ranges = 0x0
#endif
};

static inline pio_sm_config dht_program_get_default_config(uint offset) {
    pio_sm_config c = pio_get_default_sm_config();
    sm_config_set_wrap(&c, offset + dht_wrap_target, offset + dht_wrap);
    return c;
}
#endif
