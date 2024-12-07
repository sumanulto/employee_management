#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <MFRC522.h>

// OLED Configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// RC522 Pins
#define RST_PIN 5
#define SS_PIN 4

MFRC522 mfrc522(SS_PIN, RST_PIN);

// Default key for RFID authentication
MFRC522::MIFARE_Key key;

void setup() {
  Serial.begin(115200);
  SPI.begin(18, 19, 23); // SCK, MISO, MOSI
  mfrc522.PCD_Init();

  // Initialize the authentication key to default value 0xFF
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.display();

  Serial.println("Place your card to read data...");
}

void loop() {
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  byte block = 1; // Block to read
  byte buffer[18];
  byte bufferSize = sizeof(buffer);

  // Authenticate
  MFRC522::StatusCode status = mfrc522.PCD_Authenticate(
      MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print("Authentication failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  // Read data
  status = mfrc522.MIFARE_Read(block, buffer, &bufferSize);
  if (status != MFRC522::STATUS_OK) {
    Serial.print("Read failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
  } else {
    Serial.println("Data read successfully!");
    Serial.print("Data: ");
    for (byte i = 0; i < 16; i++) {
      Serial.write(buffer[i]);
    }
    Serial.println();

    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("RFID Data:");
    display.setCursor(0, 10);
    for (byte i = 0; i < 16; i++) {
      display.write(buffer[i]);
    }
    display.display();
  }

  mfrc522.PICC_HaltA();
}
