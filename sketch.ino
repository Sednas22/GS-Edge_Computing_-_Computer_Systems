// Bibliotecas obrigatórias para o hardware
#include <Wire.h> // Comunicação I2C para o LCD
#include <LiquidCrystal_I2C.h> // Controle do display LCD
#include <Keypad.h> // Leitura da matriz de botões (Keypad 4x4)

// Configuração do LCD I2C (Endereço 0x27, 16 colunas, 2 linhas)
LiquidCrystal_I2C lcd(0x27, 16, 2); 

// Definição dos pinos para Atuadores e Sensores/Entradas
#define LED_R 9 // LED RGB - Vermelho
#define LED_G 10 // LED RGB - Verde
#define LED_B 11 // LED RGB - Azul
#define BTN_VERDE 2 // Botão Verde (Avançar/Confirmar)
#define BTN_VERMELHO 3 // Botão Vermelho (Erro/Ajuda)
#define BUZZER 4 // Pino do Buzzer para feedback sonoro

// --- Configuração do Keypad 4x4 ---
const byte ROWS = 4; const byte COLS = 4;
// Mapa de caracteres do teclado
char keys[ROWS][COLS] = {
  {'1','2','3','A'}, {'4','5','6','B'}, {'7','8','9','C'}, {'*','0','#','D'}
};
// Pinos de Linha (ligados aos pinos digitais 5, 6, 7, 8)
byte rowPins[ROWS] = {5, 6, 7, 8};
// Pinos de Coluna (ligados aos pinos digitais 12, 13, A2, A3)
byte colPins[COLS] = {12, 13, A2, A3}; 
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// --- BASE DE DADOS (MPV) ---
// Estrutura de dados: [Cargo][Tarefa (1)][Passo (2)][Instrução/Detalhe (2)]
// MPV: 2 Cargos (Elétrica/Hidráulica), 1 Tarefa, 2 Passos
String passosMPV[2][1][2][2] = {
  // CARGO 1 (Index 0): ELÉTRICA
  { { {"Passo 1 Eletrica", "Desligar energia"}, {"Passo 2 Eletrica", "Testar circuito"} } },
  // CARGO 2 (Index 1): HIDRÁULICA
  { { {"Passo 1 Hidra", "Fechar o registro"}, {"Passo 2 Hidra", "Abrir o dreno"} } }
};

// --- Controle da Máquina de Estados ---
enum Estado { INICIALIZANDO, MENU_CARGO, LOGIN, MENU_TAREFA, EXECUTAR_TAREFA, MENU_FINAL };
Estado estadoAtual = INICIALIZANDO; 

// --- Variáveis de Controle ---
int cargoSelecionado = 0; // Armazena 1 (Elt) ou 2 (Hid)
int tarefaSelecionada = 0; // Armazena o número da tarefa selecionada (1 a 4)
bool menuExibido = false; // Flag para prevenir o 'flicker' nos menus

// --- Funções Auxiliares (Feedback e Display) ---

// Controla o LED RGB (Configurado como Ânodo Comum: 255 - valor)
void rgb(int r, int g, int b) { analogWrite(LED_R, 255 - r); analogWrite(LED_G, 255 - g); analogWrite(LED_B, 255 - b); }
// Bip de Confirmação
void bipOK(){ tone(BUZZER, 1200, 70); }
// Bip de Erro ou Alerta
void bipErro(){ tone(BUZZER, 400, 120); }
// Bip de Chamada de Supervisor
void bipSupervisor(){ tone(BUZZER, 200, 500); }
// Limpa o LCD e adiciona um pequeno delay para estabilidade
void lcdClear(){ lcd.clear(); delay(150); }
// Limpa o buffer do Keypad para evitar leitura indesejada (debounce manual)
void limpaKeypad(){ while(keypad.getKey()){} }

// ------------------- Funções de Estado (State Handlers) -------------------

// Estado inicial do sistema
void handleInicializando() {
    lcd.setCursor(0, 0);
    lcd.print("SAOA MPV v3.7");
    lcd.setCursor(0, 1);
    lcd.print("Aguarde...");
    delay(1500);
    estadoAtual = MENU_CARGO;
}

// Menu de Seleção de Cargo (Elétrica ou Hidráulica)
void handleMenuCargo() {
    // Evita o flicker: só desenha o menu uma vez
    if (!menuExibido) { 
        rgb(0,0,0); 
        lcdClear();
        lcd.print("1-Elt 2-Hid");
        menuExibido = true; 
    }

    char t = keypad.getKey();
    if (t == '1' || t == '2') {
        cargoSelecionado = t - '0'; // Converte char '1' ou '2' para int 1 ou 2
        limpaKeypad();
        menuExibido = false; // Reseta a flag para o próximo estado (se for um menu)
        estadoAtual = LOGIN;
    }
}

// Loop de Autenticação (Login)
void handleLogin() {
    bool loginAprovado = false;
    while (!loginAprovado) { // Loop bloqueante até o login ser aprovado
        rgb(0, 0, 0);
        lcdClear();
        lcd.print("Senha (3 dig):");
        lcd.setCursor(0, 1);
        String senha = "";
        
        // Captura 3 dígitos da senha
        for (int i = 0; i < 3; i++) {
            char t = 0;
            // Espera por um dígito válido
            do { t = keypad.getKey(); } while (t == 0 || !isDigit(t)); 
            senha += t;
            lcd.print('*'); // Exibe asterisco
            delay(150);
        }
        limpaKeypad();

        // Verifica se a senha é "123"
        if (senha.toInt() == 123) {
            bipOK();
            rgb(0, 255, 0); // Verde
            lcdClear();
            lcd.print("LOGIN OK!");
            delay(600);
            loginAprovado = true;
        } else {
            bipErro();
            rgb(255, 0, 0); // Vermelho
            lcdClear();
            lcd.print("Incorreta. Leu:");
            lcd.setCursor(0, 1);
            lcd.print(senha.toInt()); 
            delay(2000); 
        }
    }
    menuExibido = false; // Garante que o próximo menu seja desenhado
    estadoAtual = MENU_TAREFA; 
}

// Menu de Seleção de Tarefa
void handleMenuTarefa() {
    if (!menuExibido) { 
        lcdClear();
        lcd.print("Selecione tarefa");
        lcd.setCursor(0, 1);
        lcd.print("1, 2, 3 ou 4"); 
        menuExibido = true; 
    }

    char t = keypad.getKey();
    // Aceita qualquer dígito entre '1' e '4'
    if (t >= '1' && t <= '4') { 
        tarefaSelecionada = t - '0'; 
        limpaKeypad();
        menuExibido = false; 
        estadoAtual = EXECUTAR_TAREFA;
    }
}

// Lógica de Execução Passo a Passo e Tratamento de Erro (o núcleo do sistema)
void handleExecutarTarefa() {
    // MPV: Apenas 2 passos
    const int totalPassos = 2; 

    // Loop principal que itera sobre cada passo
    for (int passo = 0; passo < totalPassos; passo++) {
        int tentativas = 0; // Contador de tentativas de erro por passo
        bool passoExibido = false; // Flag para evitar flicker DENTRO do passo
        
        while (true) { // Loop que aguarda a confirmação (Botão Verde/Vermelho)
            
            // Desenha a instrução do passo uma única vez
            if (!passoExibido) {
                rgb(0, 0, 0); // LED desligado enquanto aguarda
                
                lcdClear();
                lcd.print("Passo ");
                lcd.print(passo + 1); 
                lcd.print(":");
                lcd.setCursor(0, 1);
                // Exibe a instrução principal (Index [0])
                lcd.print(passosMPV[cargoSelecionado - 1][0][passo][0]); 
                
                passoExibido = true; 
            }

            // 1. Botão VERDE (Avançar para o próximo passo)
            if (digitalRead(BTN_VERDE) == LOW) {
                bipOK();
                rgb(0, 255, 0); // Verde
                delay(300);
                limpaKeypad(); 
                break; // Sai do while(true) e avança o loop 'for'
            }

            // 2. Botão VERMELHO (Erro/Ajuda)
            if (digitalRead(BTN_VERMELHO) == LOW) {
                tentativas++;
                bipErro();
                delay(200); 
                limpaKeypad();
                
                lcdClear();
                rgb(255, 150, 0); // Laranja (aviso)
                
                // --- Lógica de Erro Hierárquica (MPV) ---
                if (passo == 0) { // Lógica para o PASSO 1
                    if (tentativas == 1) {
                        lcd.print("Dica Simples:");
                        lcd.setCursor(0, 1);
                        // Exibe o detalhe de ajuda (Index [1])
                        lcd.print(passosMPV[cargoSelecionado - 1][0][0][1]); 
                    } else { 
                        goto chamar_superior; // 2ª ou mais tentativas
                    }
                } 
                else if (passo == 1) { // Lógica para o PASSO 2
                    if (tentativas == 1) {
                        lcd.print("Verificar Passo 1"); // Lembra do passo anterior
                        lcd.setCursor(0, 1);
                        lcd.print(passosMPV[cargoSelecionado - 1][0][0][0]); 
                    } else if (tentativas == 2) {
                        lcd.print("Dica Simples:");
                        lcd.setCursor(0, 1);
                        lcd.print(passosMPV[cargoSelecionado - 1][0][1][1]); 
                    } else { 
                        goto chamar_superior; // 3ª ou mais tentativas
                    }
                }
                
                delay(1500);
                passoExibido = false; // Força o redesenho da instrução principal
            }
            delay(50); // Debounce dos botões
        }
    }
    
    // Fim da Tarefa
    lcdClear();
    lcd.print("MPV Concluido!");
    bipOK();
    rgb(0, 255, 0);
    delay(1200);
    menuExibido = false; 
    estadoAtual = MENU_FINAL;
    return;


    // --- Rotina de Chamada de Supervisor (GOTO) ---
    chamar_superior:
    rgb(255, 0, 0);
    bipSupervisor();
    lcdClear();
    lcd.print("Chame superior!");
    delay(2000);
    menuExibido = false; 
    estadoAtual = MENU_FINAL; 
    return;
}

// Menu Final: Opções após a conclusão/falha da tarefa
void handleMenuFinal() {
    if (!menuExibido) { 
        lcdClear();
        lcd.print("1-Novo 2-Cargo");
        lcd.setCursor(0, 1);
        lcd.print("3-Encerrar");
        menuExibido = true; 
    }
    
    char t = keypad.getKey();
    if (t == '1') {
        limpaKeypad();
        menuExibido = false; 
        estadoAtual = MENU_TAREFA; // Volta para seleção de tarefas
    } else if (t == '2') {
        limpaKeypad();
        menuExibido = false; 
        estadoAtual = MENU_CARGO; // Volta para seleção de cargo (requer novo login)
    } else if (t == '3') {
        lcdClear();
        lcd.print("Encerrado");
        rgb(0, 0, 0);
        while(true); // Trava o sistema
    }
}

// ------------------- Configuração Inicial (SETUP) -------------------
void setup(){
    delay(500); // Garante tempo para o LCD inicializar
    lcd.init();
    lcd.backlight();
    
    // Configuração dos pinos de saída e entradas
    pinMode(LED_R, OUTPUT); pinMode(LED_G, OUTPUT); pinMode(LED_B, OUTPUT); rgb(0, 0, 0); 
    // Configuração dos botões com pull-up interno
    pinMode(BTN_VERDE, INPUT_PULLUP); pinMode(BTN_VERMELHO, INPUT_PULLUP);
    pinMode(BUZZER, OUTPUT);
    
    lcdClear();
    delay(200);
    estadoAtual = INICIALIZANDO;
}

// ------------------- Loop Principal (LOOP) -------------------
void loop(){
    // Máquina de Estados: Chama a função responsável pelo estado atual
    switch (estadoAtual) {
        case INICIALIZANDO: handleInicializando(); break;
        case MENU_CARGO: handleMenuCargo(); break;
        case LOGIN: handleLogin(); break;
        case MENU_TAREFA: handleMenuTarefa(); break; 
        case EXECUTAR_TAREFA: handleExecutarTarefa(); break; 
        case MENU_FINAL: handleMenuFinal(); break;
    }
}
