#include "LEDController.h"

const uint8_t ledMapping[] = {3, 4, 0, 1, 2};  // Mapping logical order to physical order for RGB LEDs

LEDController::LEDController(uint8_t addr, uint16_t pwmFreq) : pwm(addr) {
  pwmFrequency = pwmFreq;
}

void LEDController::begin() {
  pwm.begin();
  pwm.setPWMFreq(pwmFrequency);
  delay(10); // Let PWM stabilize
}

void LEDController::setLED(uint8_t ledIndex, uint16_t brightness) {
  if (ledIndex < 5) {  // Handle RGB LEDs 0-4
    uint8_t mappedLedIndex = ledMapping[ledIndex];
    uint8_t baseLed = mappedLedIndex * 3;
    pwm.setPWM(baseLed, 0, 4095 - brightness);     // Red (invert logic)
    pwm.setPWM(baseLed + 1, 0, 4095 - brightness); // Green (invert logic)
    pwm.setPWM(baseLed + 2, 0, 4095 - brightness); // Blue (invert logic)
  } else if (ledIndex == 5) {  // Handle the single-color LED at index 5
    pwm.setPWM(15, 0, 4095 - brightness);  // Single LED at position LED15
  }
}

void LEDController::setRGB(uint8_t rgbIndex, uint16_t red, uint16_t green, uint16_t blue) {
  uint8_t baseLed = ledMapping[rgbIndex] * 3;
  pwm.setPWM(baseLed, 0, 4095 - red);     // Red (invert logic)
  pwm.setPWM(baseLed + 1, 0, 4095 - green); // Green (invert logic)
  pwm.setPWM(baseLed + 2, 0, 4095 - blue);  // Blue (invert logic)
}

void LEDController::turnOffRGB(uint8_t rgbIndex) {
  setRGB(rgbIndex, 0, 0, 0);  // Turn off all colors of the RGB LED by setting them to maximum PWM value
}

void LEDController::turnOffLED(uint8_t ledNumber) {
  setLED(ledNumber, 0);  // Turn off an LED by setting it to maximum PWM value
}

void LEDController::turnOff(){
  for (int i = 0;i<5;i++)
    setLED(i, 0);
}