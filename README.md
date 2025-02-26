# Alpha Segurança - Controle de segurança utilizando o Raspberry Pi Pico W RP2040

**Autor: Luis Felipe Pereira de Carvalho**

**Técnico em Analises e Desenvolvimento de Sistemas - SENAI**

**Graduando em Bacharelado de Engenharia de Computação - UEFS**

## Índice

1. [Objetivos](#objetivos)
2. [Descrição do Projeto](#descricao-do-projeto)
3. [Funcionalidades](#funcionalidades)
4. [Componentes Utilizados](#componentes-utilizados)
5. [Estrutura do Código](#estrutura-do-codigo)
6. [Melhorias Futuras](#melhorias-futuras)
7. [Documentação](#documentacao)

## Objetivos

* Criar um sistema de controle de acesso seguro e modular.
* Utilizar autenticação via senha, reconhecimento de voz e futuros métodos biométricos.
* Implementar um sistema de feedback visual e sonoro para interação do usuário.
* Permitir testes e diagnósticos dos componentes do sistema.

## Descrição do Projeto

O sistema de controle de acesso foi desenvolvido para garantir segurança em ambientes restritos. Ele permite autenticação via joystick (senha digitada manualmente), reconhecimento de voz e, simuladamente, scanner de íris. Os componentes eletrônicos interagem para fornecer um sistema robusto e confiável.

## Funcionalidades

✅ **Entrada de senha:** O usuário pode inserir uma senha utilizando um joystick.
✅ **Reconhecimento de voz:** Capacidade de autenticação através do microfone embutido.
✅ **Feedback visual e sonoro:** LEDs RGB e buzzer indicam o status do acesso.
✅ **Scanner de íris (planejado):** Uma camada extra de segurança será adicionada futuramente.
✅ **Testes e diagnósticos:** O sistema pode verificar o funcionamento dos componentes eletrônicos.
✅ **Modo de treinamento de voz:** Padrões de voz podem ser gravados e validados para futuras autenticações.

## Componentes Utilizados

* **Raspberry Pi Pico**
* **LED RGB**
* **Buzzer**
* **Joystick**
* **Microfone**
* **Matriz de LEDs**
* **Scanner de íris (planejado)**

## Estrutura do Código

```
📂 src/
 ├── debouncer.h      # debouncer para os botões
 ├── display.h      # ativa e permite o envio de dados ao display ssd1306
 ├── menu.h           # faz o processamento do menu
 ├── hardwareFiles/
 |   ├── buttons.h    # incialização dos botões
 |   ├── led_matrix.h # Controle da matriz de leds
 ├── inc/
 │   ├── ssd1306.h    # controle do display via I2C
├── main.c            # Código principal do projeto
```

## Melhorias Futuras

* Integração do scanner de íris.
* Implementação de algoritmos de machine learning para aprimorar o reconhecimento de voz.
* Adição de um display OLED para feedback mais detalhado ao usuário.
* Integração com um servidor para registro de acessos.

## Como Executar o Programa

### 1. Configuração do Ambiente

* Instale o SDK do Raspberry Pi Pico seguindo a documentação oficial.
* Configure a ferramenta de build `CMake` e o compilador ARM.

### 2. Compilação do Código

* Clone o repositório ou copie o código para um diretório local.
* No terminal, navegue até o diretório do projeto e execute:
  ```sh
  mkdir build && cd build
  cmake ..
  make
  ```

### 3. Upload para a Raspberry Pi Pico W

* Conecte a Raspberry Pi Pico ao computador enquanto segura o botão `BOOTSEL`.
* Copie o arquivo `.uf2` gerado para a unidade que aparecerá no sistema.
* A Pico será reiniciada automaticamente e executará o código.

## Documentação

A documentação detalhada do projeto, incluindo instruções de configuração, explicação dos componentes e detalhes do funcionamento do sistema, pode ser encontrada na pasta  **docs/** .

## Demonstrativo em Vídeo

Assista ao funcionamento do projeto no seguinte link:

[Vídeos no Google Drive](https://drive.google.com/drive/u/0/folders/1htpyT061CxRiVs5t-WJD19F9LfIxuQYs)
