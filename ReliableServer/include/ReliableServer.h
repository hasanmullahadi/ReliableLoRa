
#ifndef RELIABLE_SERVER_H
#define RELIABLE_SERVER_H

#include <Arduino.h>
#include <vector>
#include <cstdint>
#include <unordered_map>

#include <RHReliableDatagram.h>
#include <RH_RF95.h>
#include <vector>
#include <map>

#define DEFAULT_RF95_FREQ 433.0  // Default frequency (in MHz)

class ReliableServer {
public:
    // Constructor with hardware configuration and server address
    ReliableServer(uint8_t serverAddress, uint8_t ssPin, uint8_t rstPin, uint8_t dio0Pin, float frequency = DEFAULT_RF95_FREQ);

    // Initialize the server
    void begin();

    // Update method to be called in the main loop (non-blocking)
    void update();

    // Send a message to a specific client (non-blocking)
    void sendMessage(uint8_t clientAddress, const std::vector<uint8_t>& data);

private:
    struct ClientSession {
        unsigned long lastCommunication;  // Last timestamp of interaction
        bool waitingForAck;               // Waiting for acknowledgment status
        int retryCount;                   // Retry counter
        std::vector<uint8_t> messageQueue;  // Message queue for outgoing data
    };

    uint8_t serverAddress;          // Server address
    uint8_t ssPin, rstPin, dio0Pin; // LoRa hardware pins
    float frequency;                // Operating frequency
    RH_RF95 driver;                 // LoRa driver
    RHReliableDatagram manager;     // Reliable datagram manager

    std::map<uint8_t, ClientSession> clients;  // Client sessions map
    const unsigned long TIMEOUT = 30000;       // Client session timeout (30 seconds)
    const int MAX_RETRIES = 3;                 // Maximum retries for unacknowledged packets

    // Internal helper methods
    void initializeLoRa();                     // Handles LoRa hardware initialization
    void handleIncomingMessages();             // Non-blocking message receiver
    void manageClientTimeouts();               // Manages client session timeouts and retries
};

#endif // RELIABLE_SERVER_H
