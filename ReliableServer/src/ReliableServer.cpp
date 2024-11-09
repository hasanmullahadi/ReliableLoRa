#include "ReliableServer.h"
#include <SPI.h>

// Constructor to initialize server with hardware settings
ReliableServer::ReliableServer(uint8_t serverAddress, uint8_t ssPin, uint8_t rstPin, uint8_t dio0Pin, float frequency)
    : serverAddress(serverAddress), ssPin(ssPin), rstPin(rstPin), dio0Pin(dio0Pin), frequency(frequency), 
      driver(ssPin, dio0Pin), manager(driver, serverAddress) {}

// Begin method to initialize LoRa and ReliableDatagram
void ReliableServer::begin() {
    initializeLoRa();  // Initialize LoRa hardware and frequency

    if (!manager.init()) {
        Serial.println("RHReliableDatagram initialization failed");
        while (1); // Halt on failure
    }
}

// Internal LoRa hardware initialization
void ReliableServer::initializeLoRa() {
    // Set up LoRa reset pin and reset the module
    pinMode(rstPin, OUTPUT);
    digitalWrite(rstPin, HIGH);
    delay(10);
    digitalWrite(rstPin, LOW);
    delay(10);
    digitalWrite(rstPin, HIGH);
    delay(10);

    if (!driver.init()) {
        Serial.println("LoRa initialization failed. Check connections.");
        while (1); // Halt on failure
    }
    driver.setFrequency(frequency);
    driver.setTxPower(13, false);  // Set power output (13 dBm is a common value)

    Serial.println("LoRa initialized successfully with frequency: " + String(frequency));
}

// Non-blocking update method to handle incoming messages and client timeouts
void ReliableServer::update() {
    handleIncomingMessages();  // Check for and process any new messages
    manageClientTimeouts();    // Check client sessions and handle timeouts
}

// Handle incoming messages and automatically acknowledge clients
void ReliableServer::handleIncomingMessages() {
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    uint8_t from;

    // Poll for new messages without blocking
    if (manager.recvfromAckTimeout(buf, &len,(uint16_t) 100, &from)) {
        clients[from].lastCommunication = millis();
        clients[from].waitingForAck = false;

        Serial.print("Received message from client ");
        Serial.print(from);
        Serial.print(": ");
        Serial.println((char*)buf);

        // Send an acknowledgment back to the client
        const char* ackMessage = "ACK";
        manager.sendto((uint8_t*)ackMessage, strlen(ackMessage), from);
    }
}

// Send a message to a specific client (non-blocking)
void ReliableServer::sendMessage(uint8_t clientAddress, const std::vector<uint8_t>& data) {
    auto& client = clients[clientAddress];
    client.messageQueue = data;
    client.waitingForAck = true;
    client.retryCount = 0;

    // Attempt to send message without blocking
    manager.sendto((uint8_t*)data.data(), data.size(), clientAddress);
    client.lastCommunication = millis();
}

// Manage client timeouts and retry logic
void ReliableServer::manageClientTimeouts() {
    for (auto it = clients.begin(); it != clients.end();) {
        auto& session = it->second;
        unsigned long now = millis();

        // If waiting for acknowledgment, handle retries
        if (session.waitingForAck) {
            if (now - session.lastCommunication > 1000 && session.retryCount < MAX_RETRIES) {
                manager.sendto((uint8_t*)session.messageQueue.data(), session.messageQueue.size(), it->first);
                session.retryCount++;
                session.lastCommunication = now;
                Serial.print("Retrying message to client ");
                Serial.println(it->first);
            } else if (session.retryCount >= MAX_RETRIES) {
                Serial.print("Max retries reached, removing client ");
                Serial.println(it->first);
                it = clients.erase(it);
                continue;
            }
        }

        // Timeout clients with no recent communication
        if (now - session.lastCommunication > TIMEOUT) {
            Serial.print("Client timed out: ");
            Serial.println(it->first);
            it = clients.erase(it);
        } else {
            ++it;
        }
    }
}
