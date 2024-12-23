#ifndef ROTEL_A12_SIMULATOR_H
#define ROTEL_A12_SIMULATOR_H

#include <Arduino.h>

enum PowerState { STANDBY, ON };
enum UpdateMode { AUTO, MANUAL };
enum SourceInput {
    CD, COAX1, COAX2, OPT1, OPT2, AUX1, AUX2,
    TUNER, PHONO, USB, BLUETOOTH, PC_USB
};

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

    // Helper functions
    String freqIntToString(int _freq);
    String sourceToString();
    String formatSignedInt(int value);

public:
    RotelA12Simulator();

    // Getters
    PowerState getPowerState() const;
    int getVolumeLevel() const;
    bool getMuteState() const;
    SourceInput getCurrentSource() const;
    bool getBypassState() const;
    int getBassLevel() const;
    int getTrebleLevel() const;
    int getBalanceLevel() const;
    UpdateMode getUpdateMode() const;
    int getFrequency() const;

    // Setters
    void setPowerState(PowerState state);
    void setVolumeLevel(int vol);
    void setMuteState(bool mute);
    void setCurrentSource(SourceInput source);
    void setBypassState(bool bypass);
    void setBassLevel(int bass);
    void setTrebleLevel(int treble);
    void setBalanceLevel(int balance);
    void setUpdateMode(UpdateMode mode);
    void setFrequency(int freq);

    // Command processing
    void processCommand(String command);
};

#endif // ROTEL_A12_SIMULATOR_H
