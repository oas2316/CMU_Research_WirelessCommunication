#include <Wire.h>
#include <LoRa.h>

// Pin Definitions for LoRa
#define RFM95_CS 10   // Chip select pin (PB2 -> pin 16)
#define RFM95_RST 1   // Reset pin (PD3 -> pin 5)
#define RFM95_INT 2   // Interrupt pin (PD2 -> pin 4)

// I2C Addresses
#define ENS160_ADDRESS 0x53
#define CCS811_ADDRESS 0x5A

void setup() {
  // Serial for debugging
  Serial.begin(9600);

  // Initialize I2C
  Wire.begin();

  // Initialize ENS160
  if (!initializeENS160()) {
    Serial.println("Failed to initialize ENS160!");
  }

  // Initialize CCS811
  if (!initializeCCS811()) {
    Serial.println("Failed to initialize CCS811!");
  }

  // Initialize LoRa
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  if (!LoRa.begin(915E6)) { // Frequency: 915 MHz
    Serial.println("LoRa initialization failed!");
    while (1);
  }
  LoRa.setPins(RFM95_CS, RFM95_RST, RFM95_INT);
  Serial.println("LoRa initialized!");
}

void loop() {
  // Read data from ENS160
  uint16_t tvoc = readENS160TVOC();
  uint16_t co2 = readENS160CO2();

  // Read data from CCS811
  uint16_t ccs811TVOC = readCCS811TVOC();
  uint16_t ccs811CO2 = readCCS811CO2();

  // Transmit data via LoRa
  LoRa.beginPacket();
  LoRa.print("ENS160 TVOC: ");
  LoRa.print(tvoc);
  LoRa.print(" ppb, CO2: ");
  LoRa.print(co2);
  LoRa.print(" ppm | CCS811 TVOC: ");
  LoRa.print(ccs811TVOC);
  LoRa.print(" ppb, CO2: ");
  LoRa.print(ccs811CO2);
  LoRa.println(" ppm");
  LoRa.endPacket();

  Serial.println("Data sent over LoRa.");

  delay(5000); // Delay for 5 seconds
}

// Function to initialize ENS160
bool initializeENS160() {
  Wire.beginTransmission(ENS160_ADDRESS);
  Wire.write(0x00); // Example: Set Mode Register
  Wire.write(0x02); // Set to Standard Mode
  return (Wire.endTransmission() == 0);
}

// Function to read TVOC data from ENS160
uint16_t readENS160TVOC() {
  Wire.beginTransmission(ENS160_ADDRESS);
  Wire.write(0x22); // TVOC register (consult datasheet)
  Wire.endTransmission(false);
  Wire.requestFrom(ENS160_ADDRESS, 2);

  if (Wire.available() == 2) {
    uint16_t tvoc = Wire.read();
    tvoc |= (Wire.read() << 8);
    return tvoc;
  }
  return 0;
}

// Function to read CO2 data from ENS160
uint16_t readENS160CO2() {
  Wire.beginTransmission(ENS160_ADDRESS);
  Wire.write(0x20); // CO2 register (consult datasheet)
  Wire.endTransmission(false);
  Wire.requestFrom(ENS160_ADDRESS, 2);

  if (Wire.available() == 2) {
    uint16_t co2 = Wire.read();
    co2 |= (Wire.read() << 8);
    return co2;
  }
  return 0;
}

// Function to initialize CCS811
bool initializeCCS811() {
  Wire.beginTransmission(CCS811_ADDRESS);
  Wire.write(0xF4); // Bootloader register
  return (Wire.endTransmission() == 0);
}

// Function to read TVOC data from CCS811
uint16_t readCCS811TVOC() {
  Wire.beginTransmission(CCS811_ADDRESS);
  Wire.write(0x02); // Status register
  Wire.endTransmission(false);
  Wire.requestFrom(CCS811_ADDRESS, 2);

  if (Wire.available() == 2) {
    uint16_t tvoc = Wire.read();
    tvoc |= (Wire.read() << 8);
    return tvoc;
  }
  return 0;
}

// Function to read CO2 data from CCS811
uint16_t readCCS811CO2() {
  Wire.beginTransmission(CCS811_ADDRESS);
  Wire.write(0x01); // CO2 register
  Wire.endTransmission(false);
  Wire.requestFrom(CCS811_ADDRESS, 2);

  if (Wire.available() == 2) {
    uint16_t co2 = Wire.read();
    co2 |= (Wire.read() << 8);
    return co2;
  }
  return 0;
}
