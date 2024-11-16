#include <M5Cardputer.h>
#include <BleKeyboard.h>

// Remover definições conflitantes para evitar problemas
#undef KEY_LEFT_CTRL
#undef KEY_LEFT_SHIFT
#undef KEY_LEFT_ALT
#undef KEY_BACKSPACE
#undef KEY_TAB

#include <BleKeyboard.h>

// Inicializa o teclado Bluetooth
BleKeyboard bleKeyboard("M5Cardputer Bluetooth Keyboard", "M5Stack", 100);

// Variáveis para controle de teclas e buffer de comandos
String commandBuffer = "> ";
int cursorY = 0;
const int lineHeight = 32;
unsigned long lastKeyPressMillis = 0;
const unsigned long debounceDelay = 200;

// Definindo alguns usuários e suas senhas
struct User {
    String name;
    String password;
};

User users[] = {
    {"Usuario1", "senha123"},
    {"Usuario2", "senha456"},
    {"Usuario3", "senha789"}
};

int numUsers = sizeof(users) / sizeof(users[0]);
int selectedUser = 0;

void setup() {
    Serial.begin(115200);  // Inicializar comunicação serial
    Serial.println("Inicializando o sistema...");

    auto cfg = M5.config();
    M5Cardputer.begin(cfg, true);
    M5Cardputer.Display.setRotation(1);
    M5Cardputer.Display.setTextSize(1);  // Ajustar tamanho do texto

    // Inicializar o teclado Bluetooth
    bleKeyboard.begin();

    // Posicionar o cursor na tela
    cursorY = M5Cardputer.Display.getCursorY();

    M5Cardputer.Display.println("Sistema pronto para uso.");
    displayUserMenu();
}

void loop() {
    M5Cardputer.update();

    // Atualizar estado do teclado Bluetooth
    if (bleKeyboard.isConnected()) {
        if (M5Cardputer.Keyboard.isChange() && M5Cardputer.Keyboard.isPressed()) {
            unsigned long currentMillis = millis();
            if (currentMillis - lastKeyPressMillis >= debounceDelay) {
                lastKeyPressMillis = currentMillis;
                Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();

                // Processar teclas pressionadas
                for (auto i : status.word) {
                    commandBuffer += i;
                    M5Cardputer.Display.print(i);
                    cursorY = M5Cardputer.Display.getCursorY();
                }

                // Processar tecla de deletar
                if (status.del && commandBuffer.length() > 2) {
                    commandBuffer.remove(commandBuffer.length() - 1);
                    M5Cardputer.Display.setCursor(
                        M5Cardputer.Display.getCursorX() - 6,
                        M5Cardputer.Display.getCursorY());
                    M5Cardputer.Display.print(" ");
                    M5Cardputer.Display.setCursor(
                        M5Cardputer.Display.getCursorX() - 6,
                        M5Cardputer.Display.getCursorY());
                    cursorY = M5Cardputer.Display.getCursorY();
                }

                // Processar tecla de enter
                if (status.enter) {
                    commandBuffer.trim();
                    String message = commandBuffer.substring(2);

                    // Se estivermos na seleção de usuário, enviar a senha do usuário selecionado
                    if (commandBuffer == ">") {
                        M5Cardputer.Display.println("Escolha um usuario:");
                        displayUserMenu();
                    } else if (commandBuffer.startsWith("> ")) {
                        // Enviar a senha do usuário selecionado via Bluetooth
                        bleKeyboard.print(users[selectedUser].password);
                        M5Cardputer.Display.println("Senha enviada para " + users[selectedUser].name);
                        commandBuffer = "> ";
                        M5Cardputer.Display.print('\n');
                        cursorY = M5Cardputer.Display.getCursorY();
                    }
                }
            }
        }
    } else {
        M5Cardputer.Display.println("Aguardando conexão Bluetooth...");
        delay(1000);
    }

    // Verificar se o cursor alcançou o final da tela
    if (cursorY > M5Cardputer.Display.height() - lineHeight) {
        M5Cardputer.Display.scroll(0, -lineHeight);
        cursorY -= lineHeight;
        M5Cardputer.Display.setCursor(M5Cardputer.Display.getCursorX(), cursorY);
    }
}

// Função para exibir o menu de usuários
void displayUserMenu() {
    M5Cardputer.Display.clear();
    M5Cardputer.Display.println("Selecione um usuario:");

    for (int i = 0; i < numUsers; i++) {
        M5Cardputer.Display.print(i + 1);
        M5Cardputer.Display.print(". ");
        M5Cardputer.Display.println(users[i].name);
    }
    M5Cardputer.Display.print("> ");
    M5Cardputer.Display.println("Digite o número do usuario:");
}

// Função para selecionar o usuário
void selectUser(int userIndex) {
    if (userIndex >= 0 && userIndex < numUsers) {
        selectedUser = userIndex;
        M5Cardputer.Display.println("Usuario selecionado: " + users[selectedUser].name);
    }
}
