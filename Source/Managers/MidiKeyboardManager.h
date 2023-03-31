/*
  ==============================================================================

    This file is part of WavesTuneRTMidiController.
    Copyright (C) 2023 David Roper

    WavesTuneRTMidiController is free software: you can redistribute it and/or
    modify it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or (at your
    option) any later version.

    WavesTuneRTMidiController is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
    Public License for more details.

    You should have received a copy of the GNU General Public License along with
    WavesTuneRTMidiController. If not, see <https://www.gnu.org/licenses/>.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../Constants/ScaleConstants.h"

class MidiKeyboardManager
{
public:
    MidiKeyboardManager();
    ~MidiKeyboardManager();

    void setMidiInput(const juce::MidiDeviceInfo* const deviceInfo);

    std::function<void(const ScaleRoot&)> onScaleChange;

private:

    class MidiListener : public juce::MidiInputCallback
    {
    public:
        MidiListener(MidiKeyboardManager* manager);
        ~MidiListener();
        void handleIncomingMidiMessage(juce::MidiInput* source, const juce::MidiMessage& message) override;

    private:
        const MidiKeyboardManager* manager;
    };

    juce::AudioDeviceManager deviceManager;
    std::unique_ptr<juce::MidiDeviceInfo> midiDevice;
    MidiListener midiListener;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MidiKeyboardManager)
};
