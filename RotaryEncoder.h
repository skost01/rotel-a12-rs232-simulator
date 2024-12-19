#ifndef ROTARY_ENCODER_H
#define ROTARY_ENCODER_H

#include <Arduino.h>
#include "ButtonDebounce.h"

class RotaryEncoder {
public:
    RotaryEncoder(uint8_t pinA, uint8_t pinB, uint8_t buttonPin, unsigned long debounceDelay = 5);

    void begin();
    void update();

    int getDirection(); // -1 = Counter-clockwise, 1 = Clockwise, 0 = No movement
    bool isButtonPressed(); // Checks if the button was pressed


private:
    uint8_t pinA;
    uint8_t pinB;

    unsigned long debounceDelay;
    unsigned long lastDebounceTimeA;

    bool lastStateA;
    volatile int direction;

    ButtonDebounce button; // Using ButtonDebounce for the button functionality
};

#endif
