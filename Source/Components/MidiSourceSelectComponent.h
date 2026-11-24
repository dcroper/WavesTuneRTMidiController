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

//==============================================================================
class MidiSourceSelectComponent : public juce::Component
{
public:
    MidiSourceSelectComponent();
    ~MidiSourceSelectComponent() override;

    std::unique_ptr<juce::MidiDeviceInfo> getSelectedDevice() const;
    void setSelectedDevice(const juce::MidiDeviceInfo& device, juce::NotificationType notification = juce::sendNotificationAsync);

    // Adds a "None" choice at the top, so a device can be unselected.
    // Call before addMidiDevices().
    void addNoneItem();

    void addMidiDevices(const juce::Array<juce::MidiDeviceInfo>& devices);

    // Greys out a device so it can't be picked, shows displayText instead of
    // its name, and clears the selection if it was selected.
    void setDeviceUnavailable(const juce::MidiDeviceInfo& device, const juce::String& displayText);

    void setLabelText(juce::StringRef text);

    void paint(juce::Graphics&) override;
    void resized() override;

    std::function<void()> onChange;

private:

    // Item 1 is reserved for "None" (see addNoneItem); devices start at item 2
    static constexpr int noneItemId = 1;
    static constexpr int firstDeviceItemId = 2;

    bool hasNoneItem = false;

    juce::ComboBox deviceSelector;
    juce::Label deviceSelectorLabel;
    juce::Array<juce::MidiDeviceInfo> midiDevices;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiSourceSelectComponent)
};
