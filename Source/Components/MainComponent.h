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
#include "ScaleSelector.h"
#include "MIDISourceSelectComponent.h"
#include "../Managers/MidiOutputManager.h"
#include "../Managers/MidiKeyboardManager.h"

class MainComponent  : public juce::Component
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;

private:

    void updateMidiOutput();
    void updateScale();

    //==============================================================================
    MidiSourceSelectComponent midiKeyboardSelect;
    MidiSourceSelectComponent midiOutputSelect;
    ScaleSelector scaleSelector;
    MidiOutputManager midiOutputManager;
    MidiKeyboardManager midiKeyboardManager;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
