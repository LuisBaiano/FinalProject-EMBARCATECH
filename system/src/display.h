#ifndef DISPLAY_H
#define DISPLAY_H

#include "inc/ssd1306.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"

// Configurações do display e I2C
#define I2C_PORT       i2c1
#define I2C_SDA        14
#define I2C_SCL        15
#define DISPLAY_WIDTH  128
#define DISPLAY_HEIGHT 64
#define ENDERECO       0x3C

// Declaração externa da estrutura do display
extern ssd1306_t ssd;

// Prototipação das funções do módulo
void init_display(void);

void display_message(const char *msg1, const char *msg2, const char *msg3);

void display_off(void);

#endif // DISPLAY_H
