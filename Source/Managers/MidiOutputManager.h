/*
  ==============================================================================

    This file is part of WavesTuneRTMidiController.
    Copyright (C) 2023 David Roper

    WavesTuneRTMidiController is free software: you can redistribute it and/or
    modify it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or (at your
    option) any later version.

    WavesTuneRTMidiController is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero
    General Public License for more details.

    You should have received a copy of the GNU Affero General Public License along with
    WavesTuneRTMidiController. If not, see <https://www.gnu.org/licenses/>.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../Constants/ScaleConstants.h"

class MidiOutputManager
{
public:
    MidiOutputManager();
    ~MidiOutputManager();

    /** Creates the app's virtual MIDI output, which other apps can connect to for
        as long as this manager exists. Returns false if it can't be created, for
        example on Windows without the teVirtualMIDI driver.
    */
    bool createVirtualOutput(const juce::String& name);
    bool hasVirtualOutput() const { return virtualOutput != nullptr; }

    /** Chooses where scale messages go. The virtual output (empty identifier and
        its name) is used as it is; any other device is opened, and closed again
        when another choice is made. nullptr sends nowhere.
    */
    void setMidiOutput(const juce::MidiDeviceInfo* const deviceInfo);
    bool hasMidiOutput() const { return activeOutput != nullptr; }

    void sendScale(const Scale& scale);

    void sendScaleRoot(const ScaleRoot::Letter& scaleRootLetter, const ScaleRoot::Sign& scaleRootSign);

    void sendScaleType(const Scale::Type& scaleType);

private:
    std::unique_ptr<juce::MidiOutput> virtualOutput;  // exists for as long as the app runs
    juce::String virtualOutputName;
    std::unique_ptr<juce::MidiOutput> deviceOutput;   // the selected device, while it's selected
    juce::MidiOutput* activeOutput = nullptr;         // where messages go: one of the above, or nothing
    int scaleRootChannel;
    int scaleRootCCStart;
    int scaleRootCCRange;

    int scaleTypeChannel;
    int scaleTypeCCStart;
    int scaleTypeCCRange;
};
