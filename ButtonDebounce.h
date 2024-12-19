// ButtonDebounce.h
#ifndef BUTTON_DEBOUNCE_H
#define BUTTON_DEBOUNCE_H

#include <Arduino.h>

class ButtonDebounce {
public:
    ButtonDebounce(uint8_t buttonPin, unsigned long debounceDelay = 50);

    void begin();
    void update();

    bool isPressed(); // Returns true if the button was pressed

private:
    uint8_t buttonPin;
    unsigned long debounceDelay;
    unsigned long lastDebounceTime;
    unsigned long holdStartTime;
    bool lastState;
    bool buttonPressed;
};

#endif
