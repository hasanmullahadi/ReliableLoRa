#include <Arduino.h>

#include <ReliableClient.h>

#define LORA_SS 5
#define LORA_RST 14
#define LORA_DIO0 16
#define CLIENT_ADDRESS 1
#define SERVER_ADDRESS 2
#define RF95_FREQ 433.0  // MHz

ReliableClient client(CLIENT_ADDRESS, SERVER_ADDRESS, LORA_SS, LORA_RST, LORA_DIO0, RF95_FREQ);

void setup() {
    Serial.begin(9600);
    while (!Serial);

    client.begin();
}

void loop() {
    client.update();
    
    // Example: Send a message every 10 seconds
    static unsigned long lastSendTime = 0;
    if (millis() - lastSendTime > 10000) {
        std::vector<uint8_t> message = {'H', 'e', 'l', 'l', 'o'};
        client.sendMessage(message);
        lastSendTime = millis();
    }
}
