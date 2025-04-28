#include <Wire.h>
#include <LoRa.h>

// LoRa Pins
#define RFM95_CS 10
#define RFM95_RST 3
#define RFM95_INT 2

// I2C Addresses
#define ENS160_ADDRESS 0x53
#define CCS811_ADDRESS 0x5A

void setup() {
  Wire.begin();

  // LoRa setup
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  LoRa.setPins(RFM95_CS, RFM95_RST, RFM95_INT);
  LoRa.begin(915E6);

  initializeENS160();
  initializeCCS811();
}

void loop() {
  delay(2000);

  uint16_t ensTVOC = readENS160TVOC();
  uint16_t ensCO2 = readENS160CO2();

  uint16_t ccsTVOC = 0;
  uint16_t ccsCO2 = 0;
  readCCS811Data(ccsCO2, ccsTVOC);

  // Send data over LoRa (compact format: CSV)
  LoRa.beginPacket();
  LoRa.print("ENS160:");
  LoRa.print(ensTVOC);
  LoRa.print(",");
  LoRa.print(ensCO2);
  LoRa.print(";CCS811:");
  LoRa.print(ccsTVOC);
  LoRa.print(",");
  LoRa.print(ccsCO2);
  LoRa.endPacket();
}

// ============ ENS160 ==============

bool initializeENS160() {
  delay(100);
  Wire.beginTransmission(ENS160_ADDRESS);
  Wire.write(0x10); // Operating mode register
  Wire.write(0x02); // Standard mode
  return Wire.endTransmission() == 0;
}

uint16_t readENS160TVOC() {
  Wire.beginTransmission(ENS160_ADDRESS);
  Wire.write(0x22);
  Wire.endTransmission(false);
  Wire.requestFrom(ENS160_ADDRESS, 2);
  if (Wire.available() == 2) {
    uint8_t low = Wire.read();
    uint8_t high = Wire.read();
    return (high << 8) | low;
  }
  return 0;
}

uint16_t readENS160CO2() {
  Wire.beginTransmission(ENS160_ADDRESS);
  Wire.write(0x20);
  Wire.endTransmission(false);
  Wire.requestFrom(ENS160_ADDRESS, 2);
  if (Wire.available() == 2) {
    uint8_t low = Wire.read();
    uint8_t high = Wire.read();
    return (high << 8) | low;
  }
  return 0;
}

// ============ CCS811 ==============

bool initializeCCS811() {
  Wire.beginTransmission(CCS811_ADDRESS);
  Wire.write(0xF4); // APP_START command
  Wire.endTransmission();
  delay(100);

  Wire.beginTransmission(CCS811_ADDRESS);
  Wire.write(0x01); // MEAS_MODE register
  Wire.write(0x10); // 1s continuous measurement
  return Wire.endTransmission() == 0;
}

void readCCS811Data(uint16_t &co2, uint16_t &tvoc) {
  Wire.beginTransmission(CCS811_ADDRESS);
  Wire.write(0x02); // Status register
  Wire.endTransmission(false);
  Wire.requestFrom(CCS811_ADDRESS, 1);
  if (Wire.available() && (Wire.read() & 0x08)) {
    Wire.beginTransmission(CCS811_ADDRESS);
    Wire.write(0x00); // ALG_RESULT_DATA
    Wire.endTransmission(false);
    Wire.requestFrom(CCS811_ADDRESS, 8);
    if (Wire.available() == 8) {
      co2 = (Wire.read() << 8) | Wire.read();
      tvoc = (Wire.read() << 8) | Wire.read();
      Wire.read(); Wire.read(); Wire.read(); Wire.read();
    }
  }
}
