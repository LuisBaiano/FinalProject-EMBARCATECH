#include "src/menu.h"
#include "src/hardwareFiles/buttons.h"
#include "src/display.h"    // Para usar as funções do display e a variável 'ssd'
#include <stdio.h>

// Array estático com os itens do menu
static char *menu_items[MENU_COUNT] = {
    "STATUS",
    "DESBLOQUEAR",
    "BLOQUEAR",
    "MONITORAMENTO",
};

// Variável estática para armazenar o item selecionado (inicialmente 0)
static uint8_t selected_menu = 0;

// Inicializa o menu (pode incluir outras configurações se necessário)
void init_menu(void) {
    selected_menu = 0;
}

// Desenha o menu no display OLED
void draw_menu(void) {
    // Limpa o display
    ssd1306_fill(&ssd, false);
    ssd1306_rect(&ssd, 0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, 1, 0);
    
    // Exibe um título (ajuste a posição conforme necessário)
    ssd1306_draw_string(&ssd, "ALFA SEGURANCA", 10, 10);
    
    // Desenha cada item do menu
    for (uint8_t i = 0; i < MENU_COUNT; i++) {
        uint8_t y = 24 + i * (MENU_ITEM_HEIGHT + MENU_ITEM_SPACING);
        // Se for o item selecionado, exibe um indicador (por exemplo, "7")
        if (i == selected_menu) {
            ssd1306_draw_string(&ssd, "7", 8, y);
        } else {
            ssd1306_draw_string(&ssd, "  ", 0, y);
        }
        // Exibe o texto do item a partir da posição x=16
        ssd1306_draw_string(&ssd, menu_items[i], 20, y);
    }
    ssd1306_send_data(&ssd);
}

// Atualiza o item selecionado no menu com base na direção (negativo: cima, positivo: baixo)
void update_menu_selection(int direction) {
    if (direction < 0) {
        if (selected_menu == 0)
            selected_menu = MENU_COUNT - 1;
        else
            selected_menu--;
    } else if (direction > 0) {
        selected_menu = (selected_menu + 1) % MENU_COUNT;
    }
}
// Retorna o item atualmente selecionado
uint8_t get_selected_menu(void) {
    return selected_menu;
}
