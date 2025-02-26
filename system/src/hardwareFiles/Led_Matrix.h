#ifndef LED_MATRIX_H
#define LED_MATRIX_H

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"

// Atualiza a matriz com um número específico
void update_led_matrix(uint8_t number, PIO pio, uint sm);

// Apaga todos os LEDs da matriz
void clear_led_matrix(PIO pio, uint sm);

uint32_t matrix_rgb(double r, double g, double b);

void init_matrix(PIO pio, uint sm);

void iris_scan_test(PIO pio, uint sm, uint joystick_button_pin);

void iris_scan(PIO pio, uint sm, uint button_b);

bool get_scan_problem(void);

#endif