#include "Led_Matrix.h"
#include "hardware/pio.h"
#include "pico/stdlib.h"
#include "led_matrix.pio.h"
#include "hardware/clocks.h"
#include "src/display.h"
#include "buttons.h"
#include <stdio.h>

// Definições de pinos
#define MATRIX_WS2812_PIN     7     // Pino de controle da matriz 5x5

bool scan_problem;

/**
 * @brief Inicializa a matriz LED usando PIO
 * @param pio Instância PIO a ser utilizada
 * @param sm State machine a ser configurada
 */
void init_matrix(PIO pio, uint sm) {
    // Carrega o programa PIO na memória do PIO
    uint offset = pio_add_program(pio, &pio_matrix_program);
    // Inicializa o programa PIO para controle da matriz LED
    pio_matrix_program_init(pio, sm, offset, MATRIX_WS2812_PIN);
}

/**
 * @brief Rotina para definição da intensidade de cores do LED
 * @param b Intensidade do azul (0.0 a 1.0)
 * @param r Intensidade do vermelho (0.0 a 1.0)
 * @param g Intensidade do verde (0.0 a 1.0)
 * @return Valor de cor formatado em uint32_t
 */

uint32_t matrix_rgb(double r, double g, double b) {
    unsigned char R = r * 255;
    unsigned char G = g * 255;
    unsigned char B = b * 255;
    return (G << 24) | (R << 16) | (B << 8);
}

/**
 * @brief Apaga todos os LEDs da matriz.
 * @param pio Instância do PIO utilizada.
 * @param sm Número da state machine.
 * @details Envia o valor 0 para cada um dos 25 LEDs da matriz, desligando-os.
 */
void clear_led_matrix(PIO pio, uint sm) {
    for (int i = 0; i < 25; i++) {
        pio_sm_put_blocking(pio, sm, 0); // Envia 0 para desligar o LED
    }
}

/**
 * @brief Simula processo de leitura de íris com feedback visual
 * @param pio Instância PIO para controle da matriz
 * @param sm State machine da matriz
 * @param button_b Pino do botão para simulação de erro
 * @note Exibe padrão colorido por 3s e verifica interrupção
 */
void iris_scan(PIO pio, uint sm, uint button_b) {
    display_message("FAZENDO A", "LEITURA", "DA IRIS");
    // Frame do olho (padrão 5x5)
    const uint8_t eye_frame[25] = {
        0, 1, 1, 1, 0,
        1, 0, 0, 0, 1,
        1, 0, 1, 0, 1,
        1, 0, 0, 0, 1,
        0, 1, 1, 1, 0
    };

    // Converte cores para valores RGB
    uint32_t red   = matrix_rgb(1.0, 0.0, 0.0);
    uint32_t green = matrix_rgb(0.0, 1.0, 0.0);
    uint32_t blue  = matrix_rgb(0.0, 0.0, 1.0);
    uint32_t black = matrix_rgb(0.0, 0.0, 0.0);

    // Exibe o frame azul (padrão) de uma só vez
    for (int i = 0; i < 25; i++) {
        uint32_t color = eye_frame[i] ? blue : black;
        pio_sm_put_blocking(pio, sm, color);
    }
    busy_wait_ms(3000);  // Permite visualizar o frame completo

    // Aguarda por 3 segundos verificando se o botão B é pressionado
    bool error = false;
    uint32_t start = to_ms_since_boot(get_absolute_time());
    while (to_ms_since_boot(get_absolute_time()) - start < 3000) {
        if (gpio_get(button_b) == 0) { // Se B for pressionado, erro na leitura
            error = true;
            break;
        }
        busy_wait_ms(50);
    }

    if (error) {
        // Exibe o frame em vermelho indicando erro na leitura do iris
        for (int i = 0; i < 25; i++) {
            uint32_t color = eye_frame[i] ? red : black;
            pio_sm_put_blocking(pio, sm, color);
        }
        printf("Acesso negado!\n");
        display_message("IRIS", "NAO", "RECONHECIDA");
        busy_wait_ms(2000);
    } else {
        // Exibe o frame em verde indicando acesso concedido
        for (int i = 0; i < 25; i++) {
            uint32_t color = eye_frame[i] ? green : black;
            pio_sm_put_blocking(pio, sm, color);
        }
        printf("Acesso concedido!\n");
        display_message("IRIS", "", "RECONHECIDA");
        busy_wait_ms(2000);
    }
    clear_led_matrix(pio, sm);
}

/**
 * @brief Teste completo do sistema de leitura de íris
 * @param pio Instância PIO para controle
 * @param sm State machine da matriz
 * @param joystick_button_pin Pino para detecção de falhas
 * @note Cicla cores por 3s ou até detecção de pressionamento
 */
void iris_scan_test(PIO pio, uint sm, uint joystick_button_pin) {
    printf("Iniciando teste de varredura...\n");
    display_message("TESTANDO", "LEITOR DE", "IRIS");

    absolute_time_t start_time = get_absolute_time(); // Marca o tempo inicial

    // Resetando a flag de problema do scan no início do teste
    scan_problem = false;

    for (int r = 0; r <= 255; r += 127) {  // Reduzindo o brilho pela metade
        for (int g = 0; g <= 255; g += 127) {  
            for (int b = 0; b <= 255; b += 127) {  
                uint32_t color = matrix_rgb(r / 255.0, g / 255.0, b / 255.0);

                for (int row = 0; row < 5; row++) {
                    for (int col = 0; col < 5; col++) {
                        pio_sm_put_blocking(pio, sm, color);
                    }
                }

                busy_wait_ms(250);

                // Verifica se já passou o tempo de 3 segundos
                if (absolute_time_diff_us(start_time, get_absolute_time()) > 3000000) {
                    printf("Teste concluído em 3 segundos.\n");
                    display_message("LEITOR","", "FUNCIONANDO");
                    busy_wait_ms(500);
                    clear_led_matrix(pio, sm); // Desliga todos os LEDs ao final do teste
                    return; // Quebra o loop de cores e vai para o final
                }

                // Verifica se o botão do joystick foi pressionado
                if (gpio_get(joystick_button_pin) == 0) {
                    printf("Problema detectado no scan! Botão pressionado.\n");
                    scan_problem = true;  // Define a flag de problema
                    clear_led_matrix(pio, sm); // Desliga todos os LEDs ao final do teste
                    return;  // Sai do loop de teste imediatamente
                }
            }
        }
    }

    if (scan_problem) {
        // Mensagem indicando o problema no scan
        display_message("PROBLEMA", "NO LEITOR", "DETECTADO");
    }
    display_off();
    clear_led_matrix(pio, sm); // Desliga todos os LEDs ao final do teste
}


/**
 * @brief Obtém status de problemas no scanner de íris
 * @return true se foi detectado problema na última leitura
 */
bool get_scan_problem() {
    return scan_problem;
}