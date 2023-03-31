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

#include "MidiSourceSelectComponent.h"

//==============================================================================
MidiSourceSelectComponent::MidiSourceSelectComponent()
{
    addAndMakeVisible(deviceSelectorLabel);
    deviceSelectorLabel.setText("MIDI Device:", juce::dontSendNotification);
    deviceSelectorLabel.attachToComponent(&deviceSelector, true);
    
    addAndMakeVisible(deviceSelector);
    deviceSelector.onChange = [this] { if (onChange) onChange(); };
}

MidiSourceSelectComponent::~MidiSourceSelectComponent()
{
}

std::unique_ptr<juce::MidiDeviceInfo> MidiSourceSelectComponent::getSelectedDevice() const
{
    if (deviceSelector.getSelectedId() == 0)
    {
        return std::unique_ptr<juce::MidiDeviceInfo>();
    }
    else
    {
        return std::unique_ptr<juce::MidiDeviceInfo>(new juce::MidiDeviceInfo(midiDevices[deviceSelector.getSelectedId() - 1]));
    }
}

void MidiSourceSelectComponent::setSelectedDevice(const juce::MidiDeviceInfo& device, juce::NotificationType notification)
{
    int id = midiDevices.indexOf(device) + 1;
    deviceSelector.setSelectedId(id, notification);
}

void MidiSourceSelectComponent::addMidiDevices(juce::Array<juce::MidiDeviceInfo>& devices)
{
    midiDevices.addArray(devices);

    juce::StringArray selectorItems;
    for (auto& device : devices)
    {
        selectorItems.add(device.name);
    }

    deviceSelector.addItemList(selectorItems, 1);
}

void MidiSourceSelectComponent::setLabelText(juce::StringRef text)
{
    deviceSelectorLabel.setText(text, juce::sendNotification);
}

void MidiSourceSelectComponent::paint(juce::Graphics& g)
{
}

void MidiSourceSelectComponent::resized()
{
    auto area = getLocalBounds();
    deviceSelector.setBounds(area.removeFromRight(area.getWidth() - 150));
}
