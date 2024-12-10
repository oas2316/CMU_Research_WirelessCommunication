#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <LoRa.h>

// OLED Display Parameters
#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

// Pin Definitions for OLED
#define OLED_MOSI  11  // D1 (data pin)
#define OLED_CLK   13  // D0 (clock pin)
#define OLED_DC    8   // DC pin
#define OLED_CS    7   // CS pin
#define OLED_RESET 9   // Reset pin

// Create an OLED object
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, OLED_DC, OLED_RESET, OLED_CS);

// Pin Definitions for LoRa
#define LORA_SS 10      // Chip Select for LoRa module
#define LORA_RST 3      // Reset for LoRa module
#define LORA_DIO0 2     // DIO0 pin for LoRa module

void setup() {
  // Initialize Serial Monitor
  Serial.begin(9600);
  while (!Serial);

  Serial.println("Initializing OLED...");
  // Initialize OLED
  if (!display.begin(SSD1306_EXTERNALVCC)) {
    Serial.println("OLED initialization failed!");
    while (1); // Loop forever if OLED fails to initialize
  }
  SPI.setClockDivider(SPI_CLOCK_DIV32);

   // Set maximum brightness
  display.ssd1306_command(SSD1306_SETCONTRAST); // Command to set contrast
  display.ssd1306_command(0xFF);               // Maximum contrast value (0xFF)

  // Clear the display buffer and show startup message
  display.clearDisplay();
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);// Set text color
  display.setCursor(0, 0);            // Set cursor to top-left corner
  display.println("Waiting for LoRa data...");
  display.display();

  // Initialize LoRa
  Serial.println("Initializing LoRa...");
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0); // Set CS, Reset, DIO0
  if (!LoRa.begin(915E6)) {                  // Initialize at 915 MHz (adjust for region)
    Serial.println("LoRa initialization failed!");
    display.setCursor(0, 10);
    display.println("LoRa init failed!");
    display.display();
    while (1); // Loop forever if LoRa fails to initialize
  }
  Serial.println("LoRa initialized!");
   // Clear the display buffer
  display.clearDisplay();

  // Display a static message
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);// Set text color
  display.setCursor(0, 0);            // Set cursor to top-left corner
  display.println("Hello, SPI OLED!");// Display text
  display.println("Welcome to the Receiver PCB!");
  display.println("LoRa initialized");
  display.display();                  // Render the text on the display
}

void loop() {
  // Check for incoming LoRa packets
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String receivedText = "";

    // Read packet
    while (LoRa.available()) {
      receivedText += (char)LoRa.read();
    }

    // Display the received message
    Serial.println("Received: " + receivedText);
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.println("LoRa Receiver");
    display.setCursor(0, 10);
    display.println("Data Received:");
    display.setCursor(0, 20);
    display.println(receivedText); // Display the message
    display.display();
  }
}
