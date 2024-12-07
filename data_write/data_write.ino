#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <MFRC522.h>
#include <SPI.h>

// OLED display configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// RFID configuration
#define RST_PIN 5      // Reset pin for RC522
#define SS_PIN 4       // Slave select pin for RC522
MFRC522 rfid(SS_PIN, RST_PIN);

// Define block addresses for MIFARE Classic
#define BLOCK_ADDR 4   // Block where data will be stored

void setup() {
  Serial.begin(115200);

  // Initialize OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  display.println("Initializing...");
  display.display();

  // Initialize RFID
  SPI.begin();
  rfid.PCD_Init();
  Serial.println("Place your card near the reader to write data");

  // Clear display after setup message
  delay(2000);
  display.clearDisplay();
}

void loop() {
  // Check for RFID card presence
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    return;
  }

  // Display UID of the card
  String uid = getUID();
  Serial.println("UID: " + uid);
  
  // Authenticate the block where we want to write data
  if (authenticateBlock(rfid.uid.uidByte)) {
    // Write data to NFC card
    String name = "Suman Ghosh";
    String empID = "1";
    String data = createDataString(name, empID);

    // Write the data to the card's block
    if (writeDataToCard(data)) {
      Serial.println("Data written successfully!");
      displayMessage("Data Written!");
    } else {
      Serial.println("Failed to write data.");
      displayMessage("Write Failed!");
    }
  } else {
    Serial.println("Authentication failed.");
    displayMessage("Auth Failed!");
  }

  // Halt PICC (prevent further communication)
  rfid.PICC_HaltA();
}

// Function to read UID as a string
String getUID() {
  String uid = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    uid += String(rfid.uid.uidByte[i], HEX);
  }
  uid.toUpperCase();
  return uid;
}

// Function to create data string to write to NFC card
String createDataString(String name, String empID) {
  String data = "{\n  \"id\": \"" + empID + "\",\n  \"Name\": \"" + name + "\"\n}";
  return data;
}

// Function to authenticate and check if the card is accessible
bool authenticateBlock(byte* uid) {
  MFRC522::StatusCode status;
  
  // Define the MIFARE key object and assign the default key
  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF; // Default key is all 0xFF
  }

  // Authenticate the block with the default key
  status = rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, BLOCK_ADDR, &key, &(rfid.uid));

  if (status != MFRC522::STATUS_OK) {
    Serial.println("Authentication failed");
    return false;
  }
  return true;
}

// Function to write data to the NFC card
bool writeDataToCard(String data) {
  byte block[16]; // Block of data to write

  // Convert the data string to byte array
  data.getBytes(block, sizeof(block));
  
  MFRC522::StatusCode status = rfid.MIFARE_Write(BLOCK_ADDR, block, 16); // Write to block

  if (status == MFRC522::STATUS_OK) {
    return true;
  } else {
    return false;
  }
}

// Function to display a message on the OLED
void displayMessage(const char* message) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(10, 10);
  display.println(message);
  display.display();
}
