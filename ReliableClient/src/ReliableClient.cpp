#include "ReliableClient.h"
#include <SPI.h>

// Constructor for ReliableClient with hardware setup and server address
ReliableClient::ReliableClient(uint8_t clientAddress, uint8_t serverAddress, uint8_t ssPin, uint8_t rstPin, uint8_t dio0Pin, float frequency)
    : clientAddress(clientAddress), serverAddress(serverAddress), ssPin(ssPin), rstPin(rstPin), dio0Pin(dio0Pin),
      frequency(frequency), driver(ssPin, dio0Pin), manager(driver, clientAddress) {}

// Initialize the client by setting up LoRa
void ReliableClient::begin() {
    initializeLoRa();

    if (!manager.init()) {
        Serial.println("RHReliableDatagram initialization failed");
        while (1); // Halt on failure
    }
}

// LoRa hardware initialization
void ReliableClient::initializeLoRa() {
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

// Non-blocking update method to manage sending and acknowledgment
void ReliableClient::update() {
    switch (state) {
        case IDLE:
            // Nothing to do in IDLE state
            break;

        case SENDING:
            if (attemptSend()) {
                transitionTo(WAITING_ACK);  // Move to waiting for acknowledgment
            } else {
                Serial.println("Send failed.");
                transitionTo(IDLE);  // Move back to IDLE if unable to send
            }
            break;

        case WAITING_ACK:
            if (checkForAck()) {
                Serial.println("Acknowledgment received from server.");
                transitionTo(IDLE);  // Return to IDLE after successful acknowledgment
            } else if (millis() - lastActionTime > RETRY_TIMEOUT) {
                if (retryCount < MAX_RETRIES) {
                    Serial.println("Retrying message...");
                    retryCount++;
                    transitionTo(SENDING);  // Retry sending
                } else {
                    Serial.println("Max retries reached, giving up.");
                    transitionTo(IDLE);  // Give up after max retries
                }
            }
            break;
    }
}

// Method to send a message to the server
void ReliableClient::sendMessage(const std::vector<uint8_t>& data) {
    if (state == IDLE) {
        currentMessage = data;
        transitionTo(SENDING);  // Start the sending process
    } else {
        Serial.println("Client is busy, cannot send message.");
    }
}

// Helper method to transition between states
void ReliableClient::transitionTo(State newState) {
    state = newState;
    lastActionTime = millis();
    retryCount = 0;
}

// Attempt to send data non-blocking
bool ReliableClient::attemptSend() {
    bool success = manager.sendto((uint8_t*)currentMessage.data(), currentMessage.size(), serverAddress);
    lastActionTime = millis();
    return success;
}

// Check for acknowledgment (non-blocking)
bool ReliableClient::checkForAck() {
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    uint8_t from;

    // Check for acknowledgment from the server
    if (manager.recvfromAckTimeout(buf, &len, (uint16_t)100, &from) && from == serverAddress) {
        Serial.println("ACK received from server.");
        return true;  // Acknowledgment received
    }
    return false;  // No acknowledgment received yet
}
