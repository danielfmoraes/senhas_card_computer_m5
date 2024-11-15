#include <SD.h>
#include <SPI.h>
#include <Crypto.h>
#include <SHA256.h>
#include <BleKeyboard.h>
#include <M5Cardputer.h>

// Configuração dos pinos do SD
#define SD_CS_PIN 12
#define SD_MOSI_PIN 14
#define SD_CLK_PIN 40
#define SD_MISO_PIN 39

#define BUFFER_SIZE 64
uint8_t buffer[BUFFER_SIZE];

// Objeto do teclado Bluetooth
BleKeyboard bleKeyboard("CardComputer", "M5Stack", 100);

void calcularHashSD() {
  M5.Lcd.clear();
  M5.Lcd.setCursor(10, 10);
  M5.Lcd.println("Iniciando SD...");

  // Configurar pinos SPI manualmente
  SPI.begin(SD_CLK_PIN, SD_MISO_PIN, SD_MOSI_PIN, SD_CS_PIN);

  if (!SD.begin(SD_CS_PIN)) {
    M5.Lcd.println("Erro ao inicializar SD!");
    return;
  }

  M5.Lcd.println("SD inicializado.");

  const char *filePath = "/data.kdbx";

  if (!SD.exists(filePath)) {
    M5.Lcd.println("Arquivo data.kdbx nao encontrado!");
    return;
  }

  File file = SD.open(filePath, FILE_READ);
  if (!file) {
    M5.Lcd.println("Erro ao abrir o arquivo.");
    return;
  }

  M5.Lcd.println("Calculando hash...");

  SHA256 sha256;
  while (file.available()) {
    int bytesRead = file.read(buffer, BUFFER_SIZE); // Leia dados em blocos
    if (bytesRead > 0) {
      sha256.update(buffer, bytesRead); // Atualize com o buffer e seu tamanho
    }
  }
  file.close();

  byte hash[32];
  sha256.finalize(hash, sizeof(hash));

  M5.Lcd.println("Hash calculado:");
  for (int i = 0; i < 32; i++) {
    M5.Lcd.printf("%02x", hash[i]);
  }
}

void inicializarBluetooth() {
  M5.Lcd.clear();
  M5.Lcd.setCursor(10, 10);
  M5.Lcd.println("Iniciando Bluetooth...");

  bleKeyboard.begin();

  while (!bleKeyboard.isConnected()) {
    M5.Lcd.println("Aguardando conexao Bluetooth...");
    delay(1000);
  }
  M5.Lcd.println("Bluetooth conectado!");
}

void menuInicial() {
  M5.Lcd.clear();
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(10, 10);
  M5.Lcd.println("1. Atualizar Senhas");
  M5.Lcd.setCursor(10, 50);
  M5.Lcd.println("2. Listar Servicos");
  M5.Lcd.setCursor(10, 90);
  M5.Lcd.println("3. Configuracoes");
}

void setup() {
  // Inicialização do M5Cardputer
  M5.begin();
  M5.Lcd.clear();
  menuInicial();

  // Inicializar Bluetooth
  inicializarBluetooth();
}

void loop() {
  M5.update();

  if (M5.BtnA.wasPressed()) {
    M5.Lcd.clear();
    M5.Lcd.println("Atualizando senhas...");
    calcularHashSD(); // Chama a função para calcular o hash
    delay(2000);
    menuInicial();
  } else if (M5.BtnB.wasPressed()) {
    M5.Lcd.clear();
    M5.Lcd.println("Listando serviços...");
    delay(2000);
    menuInicial();
  } else if (M5.BtnC.wasPressed()) {
    M5.Lcd.clear();
    M5.Lcd.println("Abrindo configuracoes...");
    delay(2000);
    menuInicial();
  }

  // Teste de envio de senha via Bluetooth
  if (bleKeyboard.isConnected()) {
    bleKeyboard.print("SenhaExemplo123!"); // Envia uma senha como teste
    delay(5000);
  }
}
