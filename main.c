#include "pico/stdlib.h"
#include <stdio.h>


const uint total_slots = 10;
const uint entry_gate_pin = 15;
const uint exit_gate_pin = 16;
const uint entry_btn_pin = 14;
const uint exit_btn_pin = 13;
const uint slot_base_pin = 2;

typedef enum {
    GARAGE_NOT_FULL,
    GARAGE_FULL
} parking_state_t;

typedef enum {
    SLOT_AVAILABLE,
    SLOT_TAKEN
} parking_slot_state_t;

typedef enum {
    GATE_CLOSED,
    GATE_OPEN
} parking_gate_state_t;

 


parking_state_t parking_status = GARAGE_NOT_FULL;
parking_slot_state_t parking_slots[total_slots];
parking_gate_state_t entry_gate_status = GATE_CLOSED;
parking_gate_state_t exit_gate_status = GATE_CLOSED;


void initialize_system();

void check_parking_status();

void handle_gate(uint button_pin, parking_gate_state_t *gate_status, const char *gate_label);

void monitor_slots();

void recommend_free_slot();

bool is_button_activated(uint button_pin);

int main() {

    stdio_init_all();
    initialize_system();

    while (true) {

        check_parking_status();

        handle_gate(entry_btn_pin, &entry_gate_status, "Entry Gate");

        handle_gate(exit_btn_pin, &exit_gate_status, "Exit Gate");

        monitor_slots();
        
        sleep_ms(100); 
    }

    return 0;
}


void initialize_system() {
    for (int i = 0; i < total_slots; i++) {
        gpio_init(slot_base_pin + i);
        gpio_set_dir(slot_base_pin + i, GPIO_IN);
        gpio_pull_down(slot_base_pin + i);
        parking_slots[i] = gpio_get(slot_base_pin + i) ? SLOT_TAKEN : SLOT_AVAILABLE;
    }

    gpio_init(entry_btn_pin);
    gpio_set_dir(entry_btn_pin, GPIO_IN);
    gpio_pull_down(entry_btn_pin);

    gpio_init(exit_btn_pin);
    gpio_set_dir(exit_btn_pin, GPIO_IN);
    gpio_pull_down(exit_btn_pin);

    gpio_init(entry_gate_pin);
    gpio_set_dir(entry_gate_pin, GPIO_OUT);

    gpio_init(exit_gate_pin);
    gpio_set_dir(exit_gate_pin, GPIO_OUT);

    printf("Parking System Initialized.\n");
}


void check_parking_status() {
    int slots_occupied = 0;

    for (int i = 0; i < total_slots; i++) {
        if (parking_slots[i] == SLOT_TAKEN) {
            slots_occupied++;
        }
    }

    if (slots_occupied >= total_slots) {
        parking_status = GARAGE_FULL;
    } else {
        parking_status = GARAGE_NOT_FULL;
    }

    if (parking_status == GARAGE_FULL) {
        printf("Parking Status: FULL (%d/%d slots occupied)\n", slots_occupied, total_slots);
    } else {
        printf("Parking Status: NOT FULL (%d/%d slots occupied)\n", slots_occupied, total_slots);
    }
}

void handle_gate(uint button_pin, parking_gate_state_t *gate_status, const char *gate_label) {

    if (is_button_activated(button_pin)) {
        printf("%s: OPENING\n", gate_label);
        *gate_status = GATE_OPEN;

        if (button_pin == entry_btn_pin) {
            gpio_put(entry_gate_pin, 1);
        } else {
            gpio_put(exit_gate_pin, 1);
        }

        sleep_ms(3000); 

        printf("%s: CLOSING\n", gate_label);
        *gate_status = GATE_CLOSED;

        if (button_pin == entry_btn_pin) {
            gpio_put(entry_gate_pin, 0);
        } else {
            gpio_put(exit_gate_pin, 0);
        }
    }
}


void monitor_slots() {

    for (int i = 0; i < total_slots; i++) {
        parking_slots[i] = gpio_get(slot_base_pin + i) ? SLOT_TAKEN : SLOT_AVAILABLE;
    }
}

void recommend_free_slot() {

    for (int i = 0; i < total_slots; i++) {
        if (parking_slots[i] == SLOT_AVAILABLE) {
            printf("Recommended Slot: %d\n", i + 1);
            return;
        }
    }
    printf("No Free Slots Available.\n");
}


bool is_button_activated(uint button_pin) {

    static bool last_state[2] = {false, false};
    bool current_state = gpio_get(button_pin);

    int index = (button_pin == exit_btn_pin) ? 1 : 0;

    if (current_state && !last_state[index]) {
        last_state[index] = current_state;
        return true;
    }

    last_state[index] = current_state;
    return false;
}
