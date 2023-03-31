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

//==============================================================================
class MidiSourceSelectComponent : public juce::Component
{
public:
    MidiSourceSelectComponent();
    ~MidiSourceSelectComponent() override;

    std::unique_ptr<juce::MidiDeviceInfo> getSelectedDevice() const;
    void setSelectedDevice(const juce::MidiDeviceInfo& device, juce::NotificationType notification = juce::sendNotificationAsync);

    void addMidiDevices(juce::Array<juce::MidiDeviceInfo>& devices);

    void setLabelText(juce::StringRef text);

    void paint(juce::Graphics&) override;
    void resized() override;

    std::function<void()> onChange;

private:

    juce::ComboBox deviceSelector;
    juce::Label deviceSelectorLabel;
    juce::Array<juce::MidiDeviceInfo> midiDevices;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiSourceSelectComponent)
};
