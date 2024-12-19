#include "RotaryEncoder.h"

RotaryEncoder::RotaryEncoder(uint8_t pinA, uint8_t pinB, uint8_t buttonPin, unsigned long debounceDelay)
    : pinA(pinA), pinB(pinB), debounceDelay(debounceDelay), lastDebounceTimeA(0),
      lastStateA(HIGH), direction(0), button(buttonPin, debounceDelay) {}

void RotaryEncoder::begin() {
    pinMode(pinA, INPUT_PULLUP);
    pinMode(pinB, INPUT_PULLUP);
    button.begin();

    lastStateA = digitalRead(pinA);
}

void RotaryEncoder::update() {
    // Update button state using ButtonDebounce
    button.update();

    // Handle rotary encoder logic
    bool currentStateA = digitalRead(pinA);
    bool currentStateB = digitalRead(pinB);

    if (currentStateA != lastStateA && (millis() - lastDebounceTimeA) > debounceDelay) {
        lastDebounceTimeA = millis();
        if (currentStateA == LOW) {
            if (currentStateB == HIGH) {
                direction = 1; // Clockwise
            } else {
                direction = -1; // Counter-clockwise
            }
        }
    } else {
        direction = 0;
    }
    lastStateA = currentStateA;
}

int RotaryEncoder::getDirection() {
    int tempDirection = direction;
    direction = 0; // Reset after reading
    return tempDirection;
}

bool RotaryEncoder::isButtonPressed() {
    return button.isPressed();
}