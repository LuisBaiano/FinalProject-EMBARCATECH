#include "main.h"

// --- Variáveis globais ---
uint32_t last_interrupt_time_A = 0;
uint32_t last_interrupt_time_B = 0;
uint32_t last_interrupt_time_JOYSTICK = 0;

// --- Variáveis usada para mudança de funcionamento do sistema ---
volatile bool access_control_mode = false;
bool keypad_fault = false;
bool buzzer_fault = false;
volatile bool action_executed = false;
bool bloq_system = false;


// Para a matriz de LEDs
PIO pio = pio0;
uint sm = 0;


// Variáveis para controle de acesso
char entered_code[CODE_LENGTH + 1] = {0};
int code_index = 0;

/*==========================*/
/* Funções de Inicialização */
/*==========================*/

void lock_system(void) {
    bloq_system = true;
}

/**
 * @brief Inicializa um pino GPIO como saída para LED
 * @param pin Número do pino GPIO a ser configurado
 */
void Led_init(uint pin){
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_OUT);
    gpio_put(pin, 0);
}

/**
 * @brief Inicializa a comunicação UART com configurações padrão
 */
void uart_init_function(void) {
    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(0, GPIO_FUNC_UART);
    gpio_set_function(1, GPIO_FUNC_UART);
    uart_set_format(UART_ID, 8, 1, UART_PARITY_NONE);
    uart_set_fifo_enabled(UART_ID, true);
}

/**
 * @brief Inicializa o sistema ADC para o microfone
 */
void microphone_init(void) {
    adc_init();
    adc_gpio_init(MIC_PIN);
    adc_select_input(2);
}

void init_adc_system(void) {
    adc_init();
    adc_gpio_init(JOYSTICK_ADC_Y);
    adc_gpio_init(JOYSTICK_ADC_X);
}

/*=======================*/
/* Funções dos Buzzers   */
/*=======================*/

/**
 * @brief Gera tom em buzzer usando PWM
 * @param buzzer_pin Pino GPIO do buzzer
 * @param freq Frequência do tom em Hz
 * @param duration Duração do tom em ms
 */
const uint error_melody[] = {262, 294, 330, 349};
const uint error_durations[] = {300, 300, 300, 300};

const uint success_melody[] = {400, 500, 600};
const uint success_durations[] = {200, 200, 400};

void tocar_buzzer(uint buzzer_pin, uint freq, uint duration) {
    gpio_set_function(buzzer_pin, GPIO_FUNC_PWM);
    uint slice = pwm_gpio_to_slice_num(buzzer_pin);
    uint chan = pwm_gpio_to_channel(buzzer_pin);
    
    pwm_set_wrap(slice, 125000000 / freq);
    pwm_set_chan_level(slice, chan, DUTY_CYCLE);
    pwm_set_enabled(slice, true);
    
    busy_wait_ms(duration);
    pwm_set_enabled(slice, false);
}

/**
 * @brief Toca sequência som para erro e sucesso no buzzer
 * @param buzzer_pin Pino GPIO do buzzer
 */
void play_error(uint buzzer_pin) {
    for (int i = 0; i < sizeof(error_melody)/sizeof(error_melody[0]); i++) {
        tocar_buzzer(buzzer_pin, error_melody[i], error_durations[i]);
    }
}

void play_success(uint buzzer_pin) {
    for (int i = 0; i < sizeof(success_melody)/sizeof(success_melody[0]); i++) {
        tocar_buzzer(buzzer_pin, success_melody[i], success_durations[i]);
    }
}

/*=======================*/
/* Funções do Microfone  */
/*=======================*/

/**
 * @brief Lê valor atual do microfone via ADC
 * @return Valor lido do ADC (12-bit)
 */
uint16_t microphone_read(void) {
    return adc_read();
}


/**
 * @brief faz a detecção para confirmar o reconhecimento de voz
 */
void microphone_access(void) {
    display_message("RECONHECIMENTO", "DE", "VOZ");
    printf("Iniciando Reconhecimento de voz!\n");
    busy_wait_ms(1000);
    uint16_t adc_value = microphone_read();
    if (adc_value > SOUND_THRESHOLD) {
        printf("Som detectado. Iniciando verificação de acesso...\n");
        gpio_put(LED_RED, 1);
        busy_wait_ms(500);
        gpio_put(LED_RED, 0);
        busy_wait_ms(1000);

        if ((gpio_get(JOYSTICK_BTN) == 0)) { //usa do botão do joystick para simular voz não reconhecida
            printf("Acesso negado!\n");
            display_message("VOZ", "NAO", "RECONHECIDA");
            gpio_put(LED_RED, 1);
            busy_wait_ms(1000);
            gpio_put(LED_RED, 0);
            busy_wait_ms(1000);

        } else {
            printf("Acesso concedido!\n");
            display_message("VOZ", "RECONHECIDA", "");
            play_success(BUZZER1_PIN);
            gpio_put(LED_GREEN, 1);
            busy_wait_ms(1000);
            gpio_put(LED_GREEN, 0);
            busy_wait_ms(1000);
        }
    }
}

/*===============================*/
/* Funções de Controle de Acesso */
/*===============================*/

/**
 * @brief Processa entrada de código de acesso via UART/USB
 * @note Gerencia a interface de usuário durante a entrada do código
 */
void process_access_control(void) {
    // Enquanto o código não estiver completo, mantenha a mensagem "OBTENDO SENHA" exibida
    display_message("OBTENDO", "SENHA", "");

    // Tenta ler da USB
    int c_usb = getchar_timeout_us(0);
    if (c_usb != PICO_ERROR_TIMEOUT && c_usb >= '0' && c_usb <= '9') {
        entered_code[code_index++] = (char)c_usb;
        printf("*"); // Exibe um '*' para cada dígito
    }
    
    // Tenta ler da UART
    if (uart_is_readable(UART_ID)) {
        char c = uart_getc(UART_ID);
        if (c >= '0' && c <= '9') {
            entered_code[code_index++] = c;
            uart_putc(UART_ID, '*');
        }
    }
    
    // Se atingiu o tamanho da senha, valida-a
    if (code_index >= CODE_LENGTH) {
        entered_code[CODE_LENGTH] = '\0';
        // Atualiza a mensagem após a conclusão da digitação
        display_message("SENHA", "DIGITADA", "");
        busy_wait_ms(200);
        
        if (strcmp(entered_code, VALID_CODE) == 0) {
            printf("\nSenha Correta!\n");
            play_success(BUZZER1_PIN);
            display_message("CODIGO", "CORRETO", "");
            gpio_put(LED_RED, 0);
            gpio_put(LED_GREEN, 1);
            busy_wait_ms(1000);
            gpio_put(LED_GREEN, 0);
            
            // Após a senha correta, realiza verificação por microfone e iris
            printf("\nVerificação de voz!\n");
            microphone_access();
            busy_wait_ms(2000);
            printf("\nVerificação de iris!\n");
            iris_scan(pio, sm, BUTTON_B);
            busy_wait_ms(2000);
        } else {
            printf("\nCódigo Incorreto!\n");
            play_error(BUZZER2_PIN);
            display_message("CODIGO", "INCORRETO", "");
            gpio_put(LED_RED, 1);
            busy_wait_ms(2000);
        }
        gpio_put(LED_RED, 0);
        // Reinicia a entrada para nova tentativa
        code_index = 0;
        memset(entered_code, 0, sizeof(entered_code));
        access_control_mode = false;
    }
}

void access_control(void) {
    // Exibe a mensagem de obtenção de senha e ativa o modo de entrada
    display_message("OBTENDO", "SENHA", "");
    printf("\nDigite a senha:\n");
    code_index = 0;
    memset(entered_code, 0, sizeof(entered_code));
    access_control_mode = true;
    busy_wait_ms(2000);
}


/*================================*/
/* Funções de Teste e Diagnóstico */
/*================================*/

/**
 * @brief Inicia modo de teste do sistema
 */

void buzzer_test(void) {
    gpio_put(LED_RED, 1);
    printf("\nIniciando teste dos buzzers...\n");
    display_message("TESTANDO", "BUZZERS", "");

    printf("\nTestando buzzer 1 para som de erro...\n");
    play_error(BUZZER1_PIN);
    busy_wait_ms(1000);
    
    printf("\nTestando buzzer 2 para som de sucesso...\n");
    play_success(BUZZER2_PIN);
    busy_wait_ms(1000);

    // Simula erro se o botão do joystick for pressionado
    if (gpio_get(JOYSTICK_BTN) == 0) {
        display_message("ERRO", "NOS", "BUZZERS");
        printf("\nErro: Falha no buzzer detectada!\n");
        buzzer_fault = true;
        busy_wait_ms(2000);
    }
}

void process_keypad_test(void) {
    // Exibe a mensagem de teste apenas uma vez
    gpio_put(LED_BLUE, 1);
    printf("\nIniciando teste do teclado...\n");
    display_message("TESTANDO", "TECLADO", "");
    busy_wait_ms(2000);

    // Se o botão B for pressionado, encerra o teste
    if (gpio_get(BUTTON_B) == 0) {
        printf("Teste encerrado pelo botão B.\n");
        display_message("TESTE", "ENCERRADO", "");
        return;
    }
    // Se o botão do joystick for pressionado, simula falha
    if (gpio_get(JOYSTICK_BTN) == 0) {
        keypad_fault = true;
    }
    busy_wait_ms(3000);
}

void test_microfone(void) {
    gpio_put(LED_GREEN, 1); 
    printf("\nIniciando teste do microfone...\n");
    display_message("TESTANDO", "MICROFONE", "");
    busy_wait_ms(3000);
    
    // Realiza 5 leituras com intervalo de 500ms entre elas
    for (int i = 0; i < 5; i++) {
        uint16_t mic_val = microphone_read();
        printf("Valor ADC do microfone: %d\n", mic_val);
        
        if (mic_val > SOUND_THRESHOLD) {
            // Se o valor lido exceder o limiar, assume que um som foi detectado
            display_message("SOM", "DETECTADO", "");
        } else {
            // Caso contrário, indica que nenhum som foi detectado
            display_message("SOM", "NAO", "DETECTADO");
        }
        
        busy_wait_ms(500);
    }
    
    display_message("TESTE DO", "MICROFONE", "FINALIZADO");
    busy_wait_ms(1000);
    gpio_put(LED_BLUE, 0);
    gpio_put(LED_RED, 0);
    gpio_put(LED_GREEN, 0); 
}

void system_fault(void) {  
    if (keypad_fault) {
        printf("Erro: Problema no teclado detectado!\n");
        display_message("ERRO", "FALHA NO", "TECLADO");
    }
    if (buzzer_fault) {
        printf("Erro: Falha no buzzer detectada!\n");
        display_message("ERRO", "NO", "BUZZER");
    }
    if (get_scan_problem()){
        printf("Erro: Falha no leitor de iris detectada!\n");
        display_message("ERRO", "NO LEITOR", "DE IRIS");
    }
    if (keypad_fault || buzzer_fault || get_scan_problem()) {
        printf("Sistema travado devido a falha.\n");
        display_message("SISTEMA", "COM PROBLEMAS", "REINICIE");
        while (1) {
            gpio_put(LED_RED, 1);
            busy_wait_ms(1000);
            gpio_put(LED_RED, 0);
        }
    }
    display_message("SISTEMA", "", "OK");
    gpio_put(LED_GREEN, 1);
    printf("Sistema OK.\n");
    busy_wait_ms(1000);
    gpio_put(LED_GREEN, 0);
}

/*=========================*/
/* Funções do ADC/Joystick */
/*=========================*/

/**
 * @brief Lê valor atual de um canal do ADC
 * @param adc_channel Canal ADC a ser lido (0-3)
 * @return Valor lido do ADC (12-bit)
 */
uint16_t read_adc(uint adc_channel) {
    adc_select_input(adc_channel);
    busy_wait_ms(5);
    return adc_read();
}

int joystick_get_direction() {
    uint16_t adc_y = read_adc(0);
    if (adc_y < (ADC_CENTER - ADC_THRESHOLD))
        return 1;
    else if (adc_y > (ADC_CENTER + ADC_THRESHOLD))
        return -1;
    else
        return 0;
}

/*===========================*/
/* Callbacks e Ações do Menu */
/*===========================*/

/**
 * @brief Callback para eventos GPIO (botões e joystick)
 * @param gpio Pino que gerou o evento
 * @param events Tipo de evento detectado
 */
void gpio_callback(uint gpio, uint32_t events) {
    if (gpio == BUTTON_A) {
        if (check_debounce(&last_interrupt_time_A, DEBOUNCE_TIME)) {
            if (!action_executed) {
                action_executed = true;
                execute_menu_action(get_selected_menu());
                draw_menu();
                action_executed = false;
            }
        }
    }
    if (gpio == BUTTON_B) {
        if (check_debounce(&last_interrupt_time_B, DEBOUNCE_TIME)) {
        }
    }
    if (gpio == JOYSTICK_BTN) {
        if (check_debounce(&last_interrupt_time_JOYSTICK, DEBOUNCE_TIME)) {
        }
    }
}

/**
 * @brief Executa ação correspondente ao item do menu selecionado
 * @param menu_index Índice do item de menu selecionado
 */
void execute_menu_action(uint8_t menu_index) {
    switch (menu_index) {
        case 0:
            process_keypad_test();
            buzzer_test();
            test_microfone();
            iris_scan_test(pio, sm, JOYSTICK_BTN);
            draw_menu();
            break;
        case 1:
            access_control();
            draw_menu();
            break;
        case 2:
            lock_system();
            draw_menu();
            break;
        case 3:
            system_fault();
            break;
        default:
            break;
    }
}

/*======================*/
/* Função Main           */
/*======================*/

/**
 * @brief Função principal de inicialização e loop de controle
 * @note Gerencia todos os subsistemas e fluxo principal da aplicação
 */
int main(void) {
    stdio_init_all();
    init_buttons();
    uart_init_function();
    busy_wait_ms(2000);  // Aguarda conexão USB
    microphone_init();
    microphone_read();
    init_matrix(pio, sm);
    init_display();
    Led_init(LED_RED);
    Led_init(LED_GREEN);
    Led_init(LED_BLUE);
    init_adc_system();
    gpio_set_function(BUZZER1_PIN, GPIO_FUNC_PWM);
    gpio_set_function(BUZZER2_PIN, GPIO_FUNC_PWM);
    
    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, gpio_callback);
    gpio_set_irq_enabled_with_callback(BUTTON_B, GPIO_IRQ_EDGE_FALL, true, gpio_callback);
    gpio_set_irq_enabled_with_callback(JOYSTICK_BTN, GPIO_IRQ_EDGE_FALL, true, gpio_callback);
    
    // Loop principal: se não estivermos no modo de entrada de senha, atualiza o menu;
    // caso contrário, processa a entrada da senha (leitura via UART/USB)
// Loop principal
    draw_menu();

    while (true) {
        int dir = joystick_get_direction();
        update_menu_selection(dir);
        char unlock_code[5];  // Buffer para armazenar o código recebido (4 caracteres + null terminator)
        int code_index = 0;   // Índice para controle da digitação do código
        
        if (bloq_system) {
            display_message("SISTEMA", "", "TRAVADO");
            printf("Sistema travado pelo usuário.\n");
            gpio_put(LED_RED, 1);
            busy_wait_ms(500);
        
            code_index = 0;
            memset(unlock_code, 0, sizeof(unlock_code)); // Zera o buffer antes da leitura
        
            while (code_index < 4) {  // Continua lendo até completar 4 dígitos
                // Tenta ler da USB
                int c_usb = getchar_timeout_us(0);
                if (c_usb != PICO_ERROR_TIMEOUT && c_usb >= '0' && c_usb <= '9') {
                    unlock_code[code_index++] = (char)c_usb;
                    printf("*"); // Exibe um '*' para cada dígito
                }
        
                // Tenta ler da UART
                if (uart_is_readable(UART_ID)) {
                    char c = uart_getc(UART_ID);
                    if (c >= '0' && c <= '9') {
                        unlock_code[code_index++] = c;
                        uart_putc(UART_ID, '*'); // Exibe '*' na UART
                    }
                }
            }
        
            unlock_code[4] = '\0'; // Garante que a string termine corretamente
        
            // Verifica se o código digitado é "0000"
            if (strcmp(unlock_code, "0000") == 0) {
                gpio_put(LED_RED, 0);
                bloq_system = false;
                display_message("SISTEMA", "", "DESTRAVADO");
                printf("Sistema destravado.\n");
                gpio_put(LED_GREEN, 1);
                busy_wait_ms(1000);
                gpio_put(LED_GREEN, 0);
            }
            draw_menu();
        }
        if (access_control_mode) {
            process_access_control();
            draw_menu();
        }
        draw_menu();
        busy_wait_ms(75);
    }

return 0;
}