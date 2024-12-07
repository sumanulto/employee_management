#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Define touch sensor pins
#define TOUCH_SENSOR_1 26  // GPIO26 for the first touch sensor
#define TOUCH_SENSOR_2 27  // GPIO27 for the second touch sensor

// Define buzzer pin
#define BUZZER_PIN 25      // GPIO pin for the buzzer

void setup() {
  Serial.begin(115200);

  // Initialize touch sensor pins as inputs
  pinMode(TOUCH_SENSOR_1, INPUT);
  pinMode(TOUCH_SENSOR_2, INPUT);

  // Initialize buzzer pin as output
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  // Initialize OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  delay(2000); // Allow time for display to initialize
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  display.println("Ready!");
  display.display(); 
}

void loop() {
  // Check if the first touch sensor is touched
  if (digitalRead(TOUCH_SENSOR_1) == HIGH) {
    Serial.println("Sensor 1 touched!");
    beep(1);           // Beep 1 time
    displayMessage("Accepted"); // Display "Hello"
    delay(300);        // Prevent multiple triggers
  }

  // Check if the second touch sensor is touched
  if (digitalRead(TOUCH_SENSOR_2) == HIGH) {
    Serial.println("Sensor 2 touched!");
    beep(3);           // Beep 3 times
    displayMessage("Reject"); // Display "Bye"
    delay(300);        // Prevent multiple triggers
  }
}

// Function to beep the buzzer
void beep(int times) {
  for (int i = 0; i < times; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(200);  // Buzzer on for 200ms
    digitalWrite(BUZZER_PIN, LOW);
    delay(200);  // Buzzer off for 200ms
  }
}

// Function to display a message on the OLED
void displayMessage(const char* message) {
  display.clearDisplay();
  display.setTextSize(1);      // Set text size
  display.setTextColor(WHITE); // Set text color
  display.setCursor(0, 20);    // Set cursor position
  display.println(message);    // Print the message
  display.display();           // Show the message
}
