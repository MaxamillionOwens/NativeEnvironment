#include <SPI.h>
#include <LoRa.h>

void setup() {
  Serial.begin(9600);
  while (!Serial);
  Serial.println("LoRa Receiver");
  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) { 
    Serial.println("Received packet");
    while (LoRa.available()) {
      char incoming = (char)LoRa.read();
      Serial.print(incoming);
    }
  }
}
