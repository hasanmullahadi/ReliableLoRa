#ifndef LEDCONTROLLER_H
#define LEDCONTROLLER_H

#include <Arduino.h>
#include <Adafruit_PWMServoDriver.h>

class LEDController {
  private:
    Adafruit_PWMServoDriver pwm;
    uint16_t pwmFrequency;

  public:
    LEDController(uint8_t addr = 0x40, uint16_t pwmFreq = 1000);
    void begin();
    void setLED(uint8_t ledIndex, uint16_t brightness);
    void setRGB(uint8_t rgbIndex, uint16_t red, uint16_t green, uint16_t blue);
    void turnOffRGB(uint8_t rgbIndex);
    void turnOffLED(uint8_t ledNumber);
    void turnOff(); // turn off all LEDs
};

#endif
