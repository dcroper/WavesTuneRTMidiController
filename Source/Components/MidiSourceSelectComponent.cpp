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

#include "MidiSourceSelectComponent.h"

//==============================================================================
MidiSourceSelectComponent::MidiSourceSelectComponent()
{
    addAndMakeVisible(deviceSelectorLabel);
    deviceSelectorLabel.setText("MIDI Device:", juce::dontSendNotification);
    deviceSelectorLabel.attachToComponent(&deviceSelector, true);
    
    addAndMakeVisible(deviceSelector);
    deviceSelector.setTextWhenNothingSelected("None");
    deviceSelector.onChange = [this] { if (onChange) onChange(); };
}

MidiSourceSelectComponent::~MidiSourceSelectComponent()
{
}

std::unique_ptr<juce::MidiDeviceInfo> MidiSourceSelectComponent::getSelectedDevice() const
{
    const int index = deviceSelector.getSelectedId() - firstDeviceItemId;

    if (! juce::isPositiveAndBelow(index, midiDevices.size()))
    {
        return std::unique_ptr<juce::MidiDeviceInfo>();
    }

    return std::make_unique<juce::MidiDeviceInfo>(midiDevices[index]);
}

void MidiSourceSelectComponent::setSelectedDevice(const juce::MidiDeviceInfo& device, juce::NotificationType notification)
{
    const int index = midiDevices.indexOf(device);
    deviceSelector.setSelectedId(index >= 0 ? index + firstDeviceItemId : (hasNoneItem ? noneItemId : 0), notification);
}

void MidiSourceSelectComponent::addNoneItem()
{
    jassert(midiDevices.isEmpty());

    hasNoneItem = true;
    deviceSelector.addItem("None", noneItemId);
    deviceSelector.addSeparator();
    deviceSelector.setSelectedId(noneItemId, juce::dontSendNotification);
}

void MidiSourceSelectComponent::addMidiDevices(const juce::Array<juce::MidiDeviceInfo>& devices)
{
    const int firstNewItemId = midiDevices.size() + firstDeviceItemId;
    midiDevices.addArray(devices);

    juce::StringArray selectorItems;
    for (const auto& device : devices)
    {
        selectorItems.add(device.name);
    }

    deviceSelector.addItemList(selectorItems, firstNewItemId);
}

void MidiSourceSelectComponent::setDeviceUnavailable(const juce::MidiDeviceInfo& device, const juce::String& displayText)
{
    const int index = midiDevices.indexOf(device);
    if (index < 0)
    {
        return;
    }

    const int id = index + firstDeviceItemId;

    // Check before changing the text: getSelectedId() only reports an item
    // whose text matches what's shown.
    const bool wasSelected = deviceSelector.getSelectedId() == id;

    deviceSelector.changeItemText(id, displayText);
    deviceSelector.setItemEnabled(id, false);

    if (wasSelected)
    {
        deviceSelector.setSelectedId(hasNoneItem ? noneItemId : 0, juce::dontSendNotification);
    }
}

void MidiSourceSelectComponent::setLabelText(juce::StringRef text)
{
    deviceSelectorLabel.setText(text, juce::sendNotification);
}

void MidiSourceSelectComponent::paint(juce::Graphics&)
{
}

void MidiSourceSelectComponent::resized()
{
    auto area = getLocalBounds();
    deviceSelector.setBounds(area.removeFromRight(area.getWidth() - 150));
}
