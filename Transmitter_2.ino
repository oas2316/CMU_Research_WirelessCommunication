#include <Wire.h>
#include <LoRa.h>

// Pin Definitions for LoRa
#define RFM95_CS 10   // Chip select pin (PB2 -> pin 16)
#define RFM95_RST 3   // Reset pin (PD3 -> pin 5)
#define RFM95_INT 2   // Interrupt pin (PD2 -> pin 2)

// Pin Definitions for I2S Microphone
#define I2S_CLOCK_PIN  5   // BCLK pin (PD5)
#define I2S_WORD_SELECT_PIN 4  // LRCL pin (PD4)
#define I2S_DATA_PIN  6  // DOUT pin (PD6)

// MQ5 Sensor Analog Pin (A0)
#define MQ5_PIN A0

// Define buffer for I2S microphone data
#define BUFFER_SIZE 512
int16_t audioBuffer[BUFFER_SIZE];  // Audio data buffer

// Variables to store sensor data
float gasConcentration = 0.0;

// Function to manually read I2S audio data
void readI2SData() {
  for (int i = 0; i < BUFFER_SIZE; i++) {
    audioBuffer[i] = 0; // Reset buffer value

    // Wait for a new frame (LRCL transition)
    while (digitalRead(I2S_WORD_SELECT_PIN) == LOW);
    while (digitalRead(I2S_WORD_SELECT_PIN) == HIGH);

    // Read 16 bits of audio data
    for (int bit = 0; bit < 16; bit++) {
      if (digitalRead(I2S_DATA_PIN)) {
        audioBuffer[i] |= (1 << (15 - bit)); // Set bit if high
      }

      // Pulse the clock to read the next bit
      digitalWrite(I2S_CLOCK_PIN, HIGH);
      delayMicroseconds(1); // Adjust this based on your microphone clock speed
      digitalWrite(I2S_CLOCK_PIN, LOW);
    }
  }
}

void setup() {
  // Initialize Serial for debugging
  Serial.begin(9600);

  // Initialize LoRa
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  if (!LoRa.begin(915E6)) { // Frequency: 915 MHz
    Serial.println("LoRa initialization failed!");
    while (1);
  }
  LoRa.setPins(RFM95_CS, RFM95_RST, RFM95_INT);
  Serial.println("LoRa initialized!");

  // Initialize I2S pins
  pinMode(I2S_CLOCK_PIN, OUTPUT);
  pinMode(I2S_WORD_SELECT_PIN, INPUT);
  pinMode(I2S_DATA_PIN, INPUT);

  // Initialize MQ5 sensor
  pinMode(MQ5_PIN, INPUT);
}

void loop() {
  // Read data from MQ5 gas sensor
  gasConcentration = analogRead(MQ5_PIN); // Read raw value from MQ5

  // Capture audio data from I2S microphone
  readI2SData();

  // Transmit data via LoRa
  LoRa.beginPacket();
  LoRa.print("MQ5 Gas Concentration: ");
  LoRa.print(gasConcentration);
  LoRa.print(" | Audio Data: ");

  // For testing, we just send the first few audio buffer values
  for (int i = 0; i < 10; i++) {
    LoRa.print(audioBuffer[i]);
    LoRa.print(" ");
  }

  LoRa.endPacket();

  // Print to Serial Monitor for debugging
  Serial.print("MQ5 Gas: ");
  Serial.println(gasConcentration);
  
  Serial.print("Audio Data(first 10 samples): ");
  for (int i = 0; i < 10; i++) {
    Serial.print(audioBuffer[i]);
    Serial.print(" ");
  }
  Serial.println();

  delay(2000); // Transmit every 2 seconds
}
