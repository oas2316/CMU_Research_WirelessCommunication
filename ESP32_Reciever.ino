#include <SPI.h>
#include <LoRa.h>

// Pin Definitions for LoRa
#define LORA_SS   5    // Chip Select (CS)
#define LORA_RST  6   // Reset Pin
#define LORA_DIO0 4    // DIO0 (Interrupt)

void setup() {
    Serial.begin(115200);
    while (!Serial);

    Serial.println("Initializing LoRa Receiver...");

    LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);
    
    if (!LoRa.begin(915E6)) {  // Set frequency (adjust based on your region)
        Serial.println("LoRa initialization failed!");
        while (1);
    }

    Serial.println("LoRa Receiver Ready!");
}

void loop() {
    int packetSize = LoRa.parsePacket();
    char buffer[1024];
    int idx = 0;
    if (packetSize) {
        Serial.print("Received packet: ");
        
        // Read packet
        while (LoRa.available()) {
            char currentChar = (char) LoRa.read();
            Serial.print(currentChar);
            buffer[idx] = currentChar;
        }

        // Display RSSI (signal strength)
        Serial.print(" | RSSI: ");
        Serial.println(LoRa.packetRssi());
    }
}
