#include <Arduino.h>
#include "RotaryEncoder.h"
#include "ButtonDebounce.h"
#include "RotelA12Simulator.h"


#define pinButton 10
#define pinEncButton 3
#define pinEncPortA 6
#define pinEncPortB 2

RotaryEncoder encoder(pinEncPortB, pinEncPortA, pinEncButton); // Pin A, Pin B, Button-Pin
ButtonDebounce button(pinButton);
RotelA12Simulator rotelDevice;
String inputBuffer = "";
int volume = rotelDevice.getVolumeLevel();

void setup() {

  encoder.begin();
  button.begin();

  Serial.begin(115200);

  // Additional debugging
  while (!Serial) {
    ; // Wait for serial port to connect
  }
  Serial.print("Rotel A12 Simulator Ready$");
}

void loop() {
  if (Serial.available()) {
    char inChar = (char)Serial.read();

    // Debug: print each incoming character
    //Serial.print("Received char: ");
    //Serial.println(inChar);
    //Serial.println(inputBuffer);

    if (inChar == '!' || inChar == '?') {
      // Process the complete command
      inputBuffer += inChar;
      rotelDevice.processCommand(inputBuffer);
      inputBuffer = ""; // Clear the buffer
    } else {
      inputBuffer += inChar;
    }
  }

  // encoder changes volume level
  encoder.update();
  button.update();
  int randomFreq = random(0,8);
  int randomSource = random(0,12);

  int direction = encoder.getDirection();
  if (direction != 0) {
    if (direction == 1) {
          if (volume < 96) {
            volume = rotelDevice.getVolumeLevel();
            rotelDevice.setVolumeLevel(++volume);
            }
      } else
      {
        if (volume > 0) {
          volume = rotelDevice.getVolumeLevel();
          rotelDevice.setVolumeLevel(--volume);
          }
      }
    }

  // encoder button sets random frequency
  if (encoder.isButtonPressed()) {
      rotelDevice.setFrequency(randomFreq);
  }

  // button on pin 10 sets random source
  if (button.isPressed()) {
      rotelDevice.setCurrentSource(randomSource);
  }
}
