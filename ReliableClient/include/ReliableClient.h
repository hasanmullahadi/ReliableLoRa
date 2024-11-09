

#ifndef RELIABLE_CLIENT_H
#define RELIABLE_CLIENT_H

#include <Arduino.h>
#include <vector>
#include <cstdint>
#include <unordered_map>

#include <RHReliableDatagram.h>
#include <RH_RF95.h>
#include <vector>

#define DEFAULT_RF95_FREQ 433.0  // Default frequency in MHz

class ReliableClient {
public:
    // Constructor for ReliableClient with hardware pin configuration and server address
    ReliableClient(uint8_t clientAddress, uint8_t serverAddress, uint8_t ssPin, uint8_t rstPin, uint8_t dio0Pin, float frequency = DEFAULT_RF95_FREQ);

    // Initialize the client
    void begin();

    // Non-blocking update method to manage sending and acknowledgment
    void update();

    // Method to send a message to the server
    void sendMessage(const std::vector<uint8_t>& data);

private:
    enum State { IDLE, SENDING, WAITING_ACK };
    
    // LoRa hardware configuration
    uint8_t clientAddress;
    uint8_t serverAddress;
    uint8_t ssPin, rstPin, dio0Pin;
    float frequency;
    
    RH_RF95 driver;                // LoRa driver
    RHReliableDatagram manager;    // Reliable datagram manager

    State state = IDLE;            // Current state of the client
    std::vector<uint8_t> currentMessage;  // Message being sent
    unsigned long lastActionTime = 0;     // Time of the last action
    int retryCount = 0;                   // Retry counter

    const int MAX_RETRIES = 3;            // Maximum retries
    const unsigned long RETRY_TIMEOUT = 1000;  // Retry timeout in milliseconds

    // Helper methods
    void initializeLoRa();                // LoRa hardware initialization
    void transitionTo(State newState);    // State transition handler
    bool attemptSend();                   // Attempt to send data
    bool checkForAck();                   // Check for acknowledgment
};

#endif // RELIABLE_CLIENT_H

