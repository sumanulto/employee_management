#include <SPI.h>
#include <MFRC522.h>

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

  Serial.println("Place your card to write data...");
}

void loop() {
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  Serial.print("Card UID: ");
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    Serial.print(" ");
  }
  Serial.println();

  byte block = 1; // Block to write
  byte data[] = {"Hello ESP32!"}; // Data (max 16 bytes)

  // Authenticate
  MFRC522::StatusCode status = mfrc522.PCD_Authenticate(
      MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print("Authentication failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  // Write data
  status = mfrc522.MIFARE_Write(block, data, 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print("Write failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
  } else {
    Serial.println("Data written successfully!");
  }

  mfrc522.PICC_HaltA();
}
