#include <SPI.h>
#include <SD.h>
#include <M5Cardputer.h>

#define SD_CS_PIN 12  // Pino Chip Select para o SD

void setup() {
  // Inicializar o M5Cardputer
  M5.begin();
  M5.Lcd.clear();
  M5.Lcd.setCursor(10, 10);
  M5.Lcd.println("Iniciando o SD...");

  // Inicializar Serial para debug
  Serial.begin(115200);
  delay(1000);  // Aguarda um tempo para estabilizar a comunicação serial

  // Inicializar SPI
  SPI.begin();  // Usa o SPI padrão

  // Tenta inicializar o cartão SD
  if (!SD.begin(SD_CS_PIN)) {
    M5.Lcd.println("Erro ao inicializar o cartão SD!");
    Serial.println("Erro ao inicializar o cartão SD!");
    
    // Diagnóstico adicional: código de erro
    uint8_t cardType = SD.cardType();
    if (cardType == CARD_NONE) {
      Serial.println("Nenhum cartão SD encontrado.");
      M5.Lcd.println("Nenhum cartão SD encontrado.");
    } else {
      Serial.println("Erro desconhecido ao inicializar o SD.");
      M5.Lcd.println("Erro desconhecido.");
    }
    return;  // Se não conseguir inicializar, pare a execução
  }

  M5.Lcd.println("Cartão SD inicializado com sucesso!");
  Serial.println("Cartão SD inicializado com sucesso!");

  // Verificar se o arquivo "data.kdbx" existe
  File file = SD.open("/data.kdbx");  // Caminho absoluto para o arquivo
  if (file) {
    M5.Lcd.println("Arquivo data.kdbx encontrado!");
    Serial.println("Arquivo data.kdbx encontrado!");
    file.close();  // Fecha o arquivo após verificar
  } else {
    M5.Lcd.println("Arquivo data.kdbx não encontrado!");
    Serial.println("Arquivo data.kdbx não encontrado!");
  }
}

void loop() {
  // Código vazio por enquanto
}
