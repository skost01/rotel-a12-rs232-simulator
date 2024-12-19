// ButtonDebounce.cpp
#include "ButtonDebounce.h"

ButtonDebounce::ButtonDebounce(uint8_t buttonPin, unsigned long debounceDelay)
    : buttonPin(buttonPin), debounceDelay(debounceDelay), lastDebounceTime(0),
      lastState(HIGH), buttonPressed(false) {}

void ButtonDebounce::begin() {
    pinMode(buttonPin, INPUT_PULLUP);
    lastState = digitalRead(buttonPin);
}

void ButtonDebounce::update() {
    bool currentState = digitalRead(buttonPin);

    if (currentState != lastState && (millis() - lastDebounceTime) > debounceDelay) {
        lastDebounceTime = millis();
        if (currentState == LOW) {
            buttonPressed = true;
        } else {
            buttonPressed = false;
        }
    }
    lastState = currentState;
}

bool ButtonDebounce::isPressed() {
    bool wasPressed = buttonPressed;
    buttonPressed = false; // Reset after reading
    return wasPressed;
}
