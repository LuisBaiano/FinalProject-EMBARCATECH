#include "display.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"

// Define a estrutura do display (instância global)
ssd1306_t ssd;

/**
 * @brief Inicializa e configura o display OLED SSD1306
 * @note Configura interface I2C a 400kHz e prepara display para operação
 */
void init_display(void) {
    // Inicializa o I2C com frequência de 400kHz
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    
    // Inicializa e configura o display SSD1306
    ssd1306_init(&ssd, DISPLAY_WIDTH, DISPLAY_HEIGHT, false, ENDERECO, I2C_PORT);
    ssd1306_config(&ssd);
    ssd1306_send_data(&ssd);
    
    // Limpa o display
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);
}

/**
 * @brief Exibe mensagem de 3 linhas no display OLED
 * @param msg1 Texto da primeira linha (superior)
 * @param msg2 Texto da segunda linha (central)
 * @param msg3 Texto da terceira linha (inferior)
 * @note As mensagens são exibidas em posições fixas (10px, 20/30/40px Y)
 */
void display_message(const char *msg1, const char *msg2, const char *msg3) {
    // Limpa o display
    ssd1306_fill(&ssd, false);
    ssd1306_rect(&ssd, 0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, 1, 0);
    
    // Desenha as strings nas posições fixas
    if (msg1 != NULL) {
        ssd1306_draw_string(&ssd, msg1, 10, 20);
    }
    if (msg2 != NULL) {
        ssd1306_draw_string(&ssd, msg2, 10, 30);
    }
    if (msg3 != NULL) {
        ssd1306_draw_string(&ssd, msg3, 10, 40);
    }
    
    // Atualiza o display com os dados desenhados
    ssd1306_send_data(&ssd);
}

/**
 * @brief Desliga o display limpando seu conteúdo
 */
void display_off(void){
    // Limpa o display
    ssd1306_fill(&ssd, false);
}