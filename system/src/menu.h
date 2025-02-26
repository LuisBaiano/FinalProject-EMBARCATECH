#ifndef MENU_H
#define MENU_H

#include <stdint.h>

// Número de itens do menu e configurações de layout
#define MENU_COUNT 4
#define MENU_ITEM_HEIGHT 8     // Altura de cada item (em pixels)
#define MENU_ITEM_SPACING 2    // Espaço entre itens

// Prototipação das funções do módulo de menu
void init_menu(void);
void draw_menu(void);
void update_menu_selection(int direction);
void execute_menu_action(uint8_t menu_index);
uint8_t get_selected_menu(void);

#endif // MENU_H
