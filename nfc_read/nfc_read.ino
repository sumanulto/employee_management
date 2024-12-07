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

// Predefined RFID data
const char* employeeData[][2] = {
  {"1A2B3C4D", "Suman Ghosh"}, // Example: UID -> Name
  {"5E6F7G8H", "Ananya Roy"},
  {"9I10J11K", "Rahul Das"}
};

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
  Serial.println("Place your card near the reader");

  // Clear display after setup message
  delay(2000);
  display.clearDisplay();
}

void loop() {
  // Check for RFID card presence
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    return;
  }

  // Read UID from the card
  String uid = getUID();

  // Search for UID in predefined data
  const char* employeeName = "Unknown";
  const char* employeeID = "Unknown";
  for (int i = 0; i < sizeof(employeeData) / sizeof(employeeData[0]); i++) {
    if (uid == employeeData[i][0]) {
      employeeName = employeeData[i][1];
      employeeID = employeeData[i][0];
      break;
    }
  }

  // Display employee details on OLED
  displayDetails(employeeName, employeeID);

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
  Serial.println("UID: " + uid);
  return uid;
}

// Function to display employee details
void displayDetails(const char* name, const char* id) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(10, 10); // Adjust for centering
  display.println(name);
  display.setTextSize(1);
  display.setCursor(20, 40); // Adjust for centering
  display.print("EmpID: ");
  display.println(id);
  display.display();
}
