#include <Arduino.h>
#include "RotaryEncoder.h"
#include "ButtonDebounce.h"


#define pinButton 10
#define pinEncButton 3
#define pinEncPortA 6
#define pinEncPortB 2

RotaryEncoder encoder(pinEncPortB, pinEncPortA, pinEncButton); // Pin A, Pin B, Button-Pin
ButtonDebounce button(pinButton);

// Device state variables
enum PowerState { STANDBY, ON };
enum UpdateMode { AUTO, MANUAL };
enum SourceInput {
  CD, COAX1, COAX2, OPT1, OPT2, AUX1, AUX2,
  TUNER, PHONO, USB, BLUETOOTH, PC_USB
};
String Frequencies[8] = {"off", "32", "44.1", "48", "88.2", "96", "176.4", "192"};

class RotelA12Simulator {
private:
  PowerState powerState;
  int volumeLevel;
  bool muteState;
  SourceInput currentSource;
  bool bypassState;
  int bassLevel;
  int trebleLevel;
  int balanceLevel;
  UpdateMode updateMode;
  int frequency;

  // Existing helper functions remain the same
  String freqIntToString(int _freq) {
    String freqLevel = Frequencies[_freq];
    return freqLevel;
  }

  String sourceToString() {
    switch(currentSource) {
      case CD: return "cd";
      case COAX1: return "coax1";
      case COAX2: return "coax2";
      case OPT1: return "opt1";
      case OPT2: return "opt2";
      case AUX1: return "aux1";
      case AUX2: return "aux2";
      case TUNER: return "tuner";
      case PHONO: return "phono";
      case USB: return "usb";
      case BLUETOOTH: return "bluetooth";
      case PC_USB: return "pc_usb";
      default: return "pc_usb";
    }
  }

  String formatSignedInt(int value) {
    String result = "";
    if (value == 0) {
      result = "000";
    } else if (value > 0) {
      result += "+";
      result += (abs(value) < 10 ? "0" : "");
      result += String(abs(value));
    } else {
      result += "-";
      result += (abs(value) < 10 ? "0" : "");
      result += String(abs(value));
    }
    return result;
  }

public:
  // Constructors
  RotelA12Simulator() {
    powerState = ON;
    volumeLevel = 45;
    muteState = false;
    currentSource = PC_USB;
    bypassState = true;
    bassLevel = 0;
    trebleLevel = 0;
    balanceLevel = 0;
    updateMode = MANUAL;
    frequency = 0;
  }

  // Getters
  PowerState getPowerState() const {
    return powerState;
  }

  int getVolumeLevel() const {
    return volumeLevel;
  }

  bool getMuteState() const {
    return muteState;
  }

  SourceInput getCurrentSource() const {
    return currentSource;
  }

  bool getBypassState() const {
    return bypassState;
  }

  int getBassLevel() const {
    return bassLevel;
  }

  int getTrebleLevel() const {
    return trebleLevel;
  }

  int getBalanceLevel() const {
    return balanceLevel;
  }

  UpdateMode getUpdateMode() const {
    return updateMode;
  }

  int getFrequency() const {
    return frequency;
  }

  // Setters
  void setPowerState(PowerState state) {
    powerState = state;
    if (updateMode == AUTO) {
      Serial.print(state == ON ? "power=on$" : "power=standby$");
    }
  }

  void setVolumeLevel(int vol) {
    if (vol >= 0 && vol <= 96) {
      volumeLevel = vol;
      if (updateMode == AUTO) {
        Serial.print("volume=");
        Serial.print(volumeLevel < 10 ? "0" : "");
        Serial.print(volumeLevel);
        Serial.print("$");
      }
    }
  }

  void setMuteState(bool mute) {
    muteState = mute;
    if (updateMode == AUTO) {
      Serial.print(mute ? "mute=on$" : "mute=off$");
    }
  }

  void setCurrentSource(SourceInput source) {
    currentSource = source;
    if (updateMode == AUTO) {
      Serial.print("source=" + sourceToString() + "$");
    }
  }

  void setBypassState(bool bypass) {
    bypassState = bypass;
    if (updateMode == AUTO) {
      Serial.print(bypass ? "bypass=on$" : "bypass=off$");
    }
  }

  void setBassLevel(int bass) {
    if (bass >= -10 && bass <= 10) {
      bassLevel = bass;
      if (updateMode == AUTO) {
        Serial.print("bass=");
        Serial.print(formatSignedInt(bassLevel) + "$");
      }
    }
  }

  void setTrebleLevel(int treble) {
    if (treble >= -10 && treble <= 10) {
      trebleLevel = treble;
      if (updateMode == AUTO) {
        Serial.print("treble=");
        Serial.print(formatSignedInt(trebleLevel) + "$");
      }
    }
  }

  void setBalanceLevel(int balance) {
    if (balance >= -15 && balance <= 15) {
      balanceLevel = balance;
      if (updateMode == AUTO) {
        String signedBalanceLevel = formatSignedInt(balanceLevel);
        signedBalanceLevel.replace("+","R");
        signedBalanceLevel.replace("-","L");
        Serial.print("balance=");
        Serial.print(signedBalanceLevel + "$");
      }
    }
  }

  void setUpdateMode(UpdateMode mode) {
    updateMode = mode;
    if (mode == AUTO) {
      Serial.print("update_mode=auto$");
    } else {
      Serial.print("update_mode=manual$");
    }
  }

  void setFrequency(int freq) {
    if (freq >= 0 && freq < 8) {
      frequency = freq;
      if (updateMode == AUTO) {
        Serial.print("freq=");
        Serial.print(freqIntToString(frequency) + "$");
      }
    }
  }

  void processCommand(String command) {
    // Remove any whitespace
    command.trim();

    // Remove the terminating '!' character
    if (command.endsWith("!")) {
      command = command.substring(0, command.length() - 1);
    }

    // Power commands
    if (command == "power_on") {
      setPowerState(ON);
      return;
    }
    else if (command == "power_off") {
      setPowerState(STANDBY);
      return;
    }
    else if (command == "power_toggle") {
      setPowerState(getPowerState() == ON ? STANDBY : ON);
      return;
    }

    // RS232 Feedback commands
    if (command == "rs232_update_on") {
      setUpdateMode(AUTO);
      return;
    }
    if (command == "rs232_update_off") {
      setUpdateMode(MANUAL);
      return;
    }

    // Volume commands
    else if (command == "vol_up" && getVolumeLevel() < 96) {
      setVolumeLevel(getVolumeLevel() + 1);
      return;
    }
    else if (command == "vol_dwn" && getVolumeLevel() > 0) {
      setVolumeLevel(getVolumeLevel() - 1);
      return;
    }
    else if (command.startsWith("vol_") && command.length() == 6) {
      int newVol = command.substring(4).toInt();
      if (newVol >= 0 && newVol <= 96) {
        setVolumeLevel(newVol);
        return;
      }
    }

    // Mute commands
    else if (command == "mute") {
      setMuteState(!getMuteState());
      return;
    }
    else if (command == "mute_on") {
      setMuteState(true);
      return;
    }
    else if (command == "mute_off") {
      setMuteState(false);
      return;
    }

    // Source selection commands
    else if (command == "cd") { setCurrentSource(CD); return; }
    else if (command == "coax1") { setCurrentSource(COAX1); return; }
    else if (command == "coax2") { setCurrentSource(COAX2); return; }
    else if (command == "opt1") { setCurrentSource(OPT1); return; }
    else if (command == "opt2") { setCurrentSource(OPT2); return; }
    else if (command == "aux1") { setCurrentSource(AUX1); return; }
    else if (command == "aux2") { setCurrentSource(AUX2); return; }
    else if (command == "tuner") { setCurrentSource(TUNER); return; }
    else if (command == "phono") { setCurrentSource(PHONO); return; }
    else if (command == "usb") { setCurrentSource(USB); return; }
    else if (command == "bluetooth") { setCurrentSource(BLUETOOTH); return; }
    else if (command == "pcusb") { setCurrentSource(PC_USB); return; }

    // Tone bypass commands
    else if (command == "bypass_on") {
      setBypassState(true);
      return;
    }
    else if (command == "bypass_off") {
      setBypassState(false);
      return;
    }

    // Bass commands with new format
    else if (command.startsWith("bass_")) {
      String valueStr = command.substring(5);
      if (valueStr.startsWith("+") || valueStr.startsWith("-") || valueStr == "000") {
        int newBassLevel = valueStr.toInt();
        if (newBassLevel >= -10 && newBassLevel <= 10) {
          setBassLevel(newBassLevel);
          return;
        }
      } else if (valueStr == "up") {
        setBassLevel(getBassLevel() + 1);
        return;
      } else if (valueStr == "down") {
        setBassLevel(getBassLevel() - 1);
        return;
      }
    }

    // Treble commands with new format
    else if (command.startsWith("treble_")) {
      String valueStr = command.substring(7);
      if (valueStr.startsWith("+") || valueStr.startsWith("-") || valueStr == "000") {
        int newTrebleLevel = valueStr.toInt();
        if (newTrebleLevel >= -10 && newTrebleLevel <= 10) {
          setTrebleLevel(newTrebleLevel);
          return;
        }
      } else if (valueStr == "up") {
        setTrebleLevel(getTrebleLevel() + 1);
        return;
      } else if (valueStr == "down") {
        setTrebleLevel(getTrebleLevel() - 1);
        return;
      }
    }

    // Balance commands with new format
    else if (command.startsWith("balance_")) {
      String valueStr = command.substring(8);
      valueStr.toUpperCase();
      int newBalanceLevel = valueStr.substring(1).toInt();
      if (newBalanceLevel >= 0 && newBalanceLevel <= 15) {
        int balanceValue = (valueStr.startsWith("R") ? newBalanceLevel : -newBalanceLevel);
        setBalanceLevel(balanceValue);
        return;
      }
    }

    // Feedback request commands
    else if (command == "power?") {
      Serial.print(getPowerState() == ON ? "power=on$" : "power=standby$");
      return;
    }
    else if (command == "source?") {
      Serial.print("source=" + sourceToString() + "$");
      return;
    }
    else if (command == "volume?") {
      int vol = getVolumeLevel();
      Serial.print("volume=");
      Serial.print(vol < 10 ? "0" : "");
      Serial.print(vol);
      Serial.print("$");
      return;
    }
    else if (command == "mute?") {
      Serial.print(getMuteState() ? "mute=on$" : "mute=off$");
      return;
    }
    else if (command == "bypass?") {
      Serial.print(getBypassState() ? "bypass=on$" : "bypass=off$");
      return;
    }
    else if (command == "bass?") {
      Serial.print("bass=");
      Serial.print(formatSignedInt(getBassLevel()) + "$");
      return;
    }
    else if (command == "treble?") {
      Serial.print("treble=");
      Serial.print(formatSignedInt(getTrebleLevel()) + "$");
      return;
    }
    else if (command == "balance?") {
      int balanceLevel = getBalanceLevel();
      String signedBalanceLevel = formatSignedInt(balanceLevel);
      signedBalanceLevel.replace("+","R");
      signedBalanceLevel.replace("-","L");
      Serial.print("balance=");
      Serial.print(signedBalanceLevel + "$");
      return;
    }
    else if (command == "freq?") {
      Serial.print("freq=");
      Serial.print(freqIntToString(getFrequency()) + "$");
      return;
    }

    // If no command matched
    Serial.print("Error: Unknown command$");
  }
}
;

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
