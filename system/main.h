#ifndef MAIN
#define MAIN

#include <stdio.h>
#include <string.h>
#include "src/hardwareFiles/buttons.h"
#include "src/debouncer.h"
#include "hardware/uart.h"
#include "hardware/i2c.h"
#include "hardware/clocks.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include "src/hardwareFiles/Led_Matrix.h"
#include "src/display.h"
#include "src/menu.h"

// --- Definições de acesso ---
#define VALID_CODE "1234"
#define CODE_LENGTH 4

// --- Definições dos LEDs RGB ---
#define LED_GREEN  11    // componente verde
#define LED_RED    13    // componente vermelho
#define LED_BLUE   12    // componente azul

// --- Definições dos pinos ---
#define BUZZER1_PIN 10
#define BUZZER2_PIN 21
#define UART_ID uart0
#define BAUD_RATE 115200

// --- Outras definições ---
#define ADC_CENTER     2048
#define ADC_THRESHOLD  512   // Limiar para detectar movimento do joystick
#define SOUND_THRESHOLD 1900  // Limiar para detecção de som

#define JOYSTICK_ADC_X 26    // ADC canal 0: eixo X
#define JOYSTICK_ADC_Y 27    // ADC canal 1: eixo Y

#define MATRIX_WS2812_PIN 7  // Pino de controle da matriz 5x5
#define MIC_PIN 28           // Microfone (ADC canal 2)
#define DEBOUNCE_TIME 300000 // Tempo de debounce (em microsegundos)
#define DUTY_CYCLE 49152



#endif