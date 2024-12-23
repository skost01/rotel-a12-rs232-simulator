#include "RotelA12Simulator.h"

const String Frequencies[8] = {"off", "32", "44.1", "48", "88.2", "96", "176.4", "192"};

String RotelA12Simulator::freqIntToString(int _freq) {
    return Frequencies[_freq];
}

String RotelA12Simulator::sourceToString() {
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

String RotelA12Simulator::formatSignedInt(int value) {
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

RotelA12Simulator::RotelA12Simulator() {
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

// Getters implementation
PowerState RotelA12Simulator::getPowerState() const { return powerState; }
int RotelA12Simulator::getVolumeLevel() const { return volumeLevel; }
bool RotelA12Simulator::getMuteState() const { return muteState; }
SourceInput RotelA12Simulator::getCurrentSource() const { return currentSource; }
bool RotelA12Simulator::getBypassState() const { return bypassState; }
int RotelA12Simulator::getBassLevel() const { return bassLevel; }
int RotelA12Simulator::getTrebleLevel() const { return trebleLevel; }
int RotelA12Simulator::getBalanceLevel() const { return balanceLevel; }
UpdateMode RotelA12Simulator::getUpdateMode() const { return updateMode; }
int RotelA12Simulator::getFrequency() const { return frequency; }

// Setters implementation
void RotelA12Simulator::setPowerState(PowerState state) {
    powerState = state;
    if (updateMode == AUTO) {
        Serial.print(state == ON ? "power=on$" : "power=standby$");
    }
}

void RotelA12Simulator::setVolumeLevel(int vol) {
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

void RotelA12Simulator::setMuteState(bool mute) {
    muteState = mute;
    if (updateMode == AUTO) {
        Serial.print(mute ? "mute=on$" : "mute=off$");
    }
}

void RotelA12Simulator::setCurrentSource(SourceInput source) {
    currentSource = source;
    if (updateMode == AUTO) {
        Serial.print("source=" + sourceToString() + "$");
    }
}

void RotelA12Simulator::setBypassState(bool bypass) {
    bypassState = bypass;
    if (updateMode == AUTO) {
        Serial.print(bypass ? "bypass=on$" : "bypass=off$");
    }
}

void RotelA12Simulator::setBassLevel(int bass) {
    if (bass >= -10 && bass <= 10) {
        bassLevel = bass;
        if (updateMode == AUTO) {
            Serial.print("bass=");
            Serial.print(formatSignedInt(bassLevel) + "$");
        }
    }
}

void RotelA12Simulator::setTrebleLevel(int treble) {
    if (treble >= -10 && treble <= 10) {
        trebleLevel = treble;
        if (updateMode == AUTO) {
            Serial.print("treble=");
            Serial.print(formatSignedInt(trebleLevel) + "$");
        }
    }
}

void RotelA12Simulator::setBalanceLevel(int balance) {
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

void RotelA12Simulator::setUpdateMode(UpdateMode mode) {
    updateMode = mode;
    if (mode == AUTO) {
        Serial.print("update_mode=auto$");
    } else {
        Serial.print("update_mode=manual$");
    }
}

void RotelA12Simulator::setFrequency(int freq) {
    if (freq >= 0 && freq < 8) {
        frequency = freq;
        if (updateMode == AUTO) {
            Serial.print("freq=");
            Serial.print(freqIntToString(frequency) + "$");
        }
    }
}

void RotelA12Simulator::processCommand(String command) {
    command.trim();
    
    if (command.endsWith("!")) {
        command = command.substring(0, command.length() - 1);
    }

    // Power commands
    if (command == "power_on") {
        setPowerState(ON);
    }
    else if (command == "power_off") {
        setPowerState(STANDBY);
    }
    else if (command == "power_toggle") {
        setPowerState(getPowerState() == ON ? STANDBY : ON);
    }
    // RS232 Feedback commands
    else if (command == "rs232_update_on") {
        setUpdateMode(AUTO);
    }
    else if (command == "rs232_update_off") {
        setUpdateMode(MANUAL);
    }
    // Volume commands
    else if (command == "vol_up" && getVolumeLevel() < 96) {
        setVolumeLevel(getVolumeLevel() + 1);
    }
    else if (command == "vol_dwn" && getVolumeLevel() > 0) {
        setVolumeLevel(getVolumeLevel() - 1);
    }
    else if (command.startsWith("vol_") && command.length() == 6) {
        int newVol = command.substring(4).toInt();
        if (newVol >= 0 && newVol <= 96) {
            setVolumeLevel(newVol);
        }
    }
    // Mute commands
    else if (command == "mute") {
        setMuteState(!getMuteState());
    }
    else if (command == "mute_on") {
        setMuteState(true);
    }
    else if (command == "mute_off") {
        setMuteState(false);
    }
    // Source selection commands
    else if (command == "cd") { setCurrentSource(CD); }
    else if (command == "coax1") { setCurrentSource(COAX1); }
    else if (command == "coax2") { setCurrentSource(COAX2); }
    else if (command == "opt1") { setCurrentSource(OPT1); }
    else if (command == "opt2") { setCurrentSource(OPT2); }
    else if (command == "aux1") { setCurrentSource(AUX1); }
    else if (command == "aux2") { setCurrentSource(AUX2); }
    else if (command == "tuner") { setCurrentSource(TUNER); }
    else if (command == "phono") { setCurrentSource(PHONO); }
    else if (command == "usb") { setCurrentSource(USB); }
    else if (command == "bluetooth") { setCurrentSource(BLUETOOTH); }
    else if (command == "pcusb") { setCurrentSource(PC_USB); }
    // Tone bypass commands
    else if (command == "bypass_on") {
        setBypassState(true);
    }
    else if (command == "bypass_off") {
        setBypassState(false);
    }
    // Bass commands
    else if (command.startsWith("bass_")) {
        String valueStr = command.substring(5);
        if (valueStr.startsWith("+") || valueStr.startsWith("-") || valueStr == "000") {
            int newBassLevel = valueStr.toInt();
            if (newBassLevel >= -10 && newBassLevel <= 10) {
                setBassLevel(newBassLevel);
            }
        } else if (valueStr == "up") {
            setBassLevel(getBassLevel() + 1);
        } else if (valueStr == "down") {
            setBassLevel(getBassLevel() - 1);
        }
    }
    // Treble commands
    else if (command.startsWith("treble_")) {
        String valueStr = command.substring(7);
        if (valueStr.startsWith("+") || valueStr.startsWith("-") || valueStr == "000") {
            int newTrebleLevel = valueStr.toInt();
            if (newTrebleLevel >= -10 && newTrebleLevel <= 10) {
                setTrebleLevel(newTrebleLevel);
            }
        } else if (valueStr == "up") {
            setTrebleLevel(getTrebleLevel() + 1);
        } else if (valueStr == "down") {
            setTrebleLevel(getTrebleLevel() - 1);
        }
    }
    // Balance commands
    else if (command.startsWith("balance_")) {
        String valueStr = command.substring(8);
        valueStr.toUpperCase();
        int newBalanceLevel = valueStr.substring(1).toInt();
        if (newBalanceLevel >= 0 && newBalanceLevel <= 15) {
            int balanceValue = (valueStr.startsWith("R") ? newBalanceLevel : -newBalanceLevel);
            setBalanceLevel(balanceValue);
        }
    }
    // Feedback request commands
    else if (command == "power?") {
        Serial.print(getPowerState() == ON ? "power=on$" : "power=standby$");
    }
    else if (command == "source?") {
        Serial.print("source=" + sourceToString() + "$");
    }
    else if (command == "volume?") {
        int vol = getVolumeLevel();
        Serial.print("volume=");
        Serial.print(vol < 10 ? "0" : "");
        Serial.print(vol);
        Serial.print("$");
    }
    else if (command == "mute?") {
        Serial.print(getMuteState() ? "mute=on$" : "mute=off$");
    }
    else if (command == "bypass?") {
        Serial.print(getBypassState() ? "bypass=on$" : "bypass=off$");
    }
    else if (command == "bass?") {
        Serial.print("bass=");
        Serial.print(formatSignedInt(getBassLevel()) + "$");
    }
    else if (command == "treble?") {
        Serial.print("treble=");
        Serial.print(formatSignedInt(getTrebleLevel()) + "$");
    }
    else if (command == "balance?") {
        int balanceLevel = getBalanceLevel();
        String signedBalanceLevel = formatSignedInt(balanceLevel);
        signedBalanceLevel.replace("+","R");
        signedBalanceLevel.replace("-","L");
        Serial.print("balance=");
        Serial.print(signedBalanceLevel + "$");
    }
    else if (command == "freq?") {
        Serial.print("freq=");
        Serial.print(freqIntToString(getFrequency()) + "$");
    }
    else {
        Serial.print("Error: Unknown command$");
    }
}
