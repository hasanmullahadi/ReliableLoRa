#include <Arduino.h>

#include <ReliableServer.h>

#include "LEDController.h"

#define LED_DRIVER_ADDR 0x40

LEDController ledController(LED_DRIVER_ADDR);


#define LORA_SS 5
#define LORA_RST 14
#define LORA_DIO0 16
#define RF95_FREQ 433.0  // MHz

#define SERVER_ADDRESS 2

ReliableServer server(SERVER_ADDRESS, LORA_SS, LORA_RST, LORA_DIO0, RF95_FREQ);

void setup() {
   ledController.begin();
    for (int i = 0; i < 6; i++)
        ledController.setLED(i, 50);


    Serial.begin(9600);
    while (!Serial);

    server.begin();
}

void loop() {
    server.update();
    // Other non-blocking tasks can be added here
}
