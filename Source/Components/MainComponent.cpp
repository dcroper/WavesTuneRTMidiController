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

#include "../Constants/ScaleConstants.h"
#include "MainComponent.h"
#include "../Settings/DeviceChoices.h"

//==============================================================================
static const juce::MidiDeviceInfo VIRTUAL_MIDI_OUTPUT_DEVICE("WavesTuneRTVirtualMIDI", "");

#if JUCE_WINDOWS
 // Windows needs the teVirtualMIDI driver, which comes with loopMIDI
 static const juce::String VIRTUAL_MIDI_UNAVAILABLE_TEXT("WavesTuneRTVirtualMIDI (install loopMIDI to enable)");
#else
 static const juce::String VIRTUAL_MIDI_UNAVAILABLE_TEXT("WavesTuneRTVirtualMIDI (unavailable)");
#endif

MainComponent::MainComponent(SettingsStore& settingsToUse)
  : settings(settingsToUse)
{
    addAndMakeVisible(midiKeyboardSelect);
    midiKeyboardSelect.setLabelText("MIDI Keyboard:");
    midiKeyboardSelect.addNoneItem();
    midiKeyboardSelect.addMidiDevices(juce::MidiInput::getAvailableDevices());
    restoreMidiKeyboard();
    midiKeyboardSelect.onChange = [this]
    {
        midiKeyboardManager.setMidiInput(midiKeyboardSelect.getSelectedDevice().get());
        saveMidiKeyboard();
    };

    // Create the virtual output once. It stays until the app quits; picking it
    // in the list only chooses where messages go.
    const bool virtualOutputAvailable = midiOutputManager.createVirtualOutput(VIRTUAL_MIDI_OUTPUT_DEVICE.name);

    addAndMakeVisible(midiOutputSelect);
    midiOutputSelect.setLabelText("MIDI Output:");
    juce::Array<juce::MidiDeviceInfo> midiOutputDevices;
    midiOutputDevices.add(VIRTUAL_MIDI_OUTPUT_DEVICE);
    midiOutputDevices.addArray(juce::MidiOutput::getAvailableDevices());

    if (virtualOutputAvailable)
    {
        midiOutputSelect.addMidiDevices(midiOutputDevices);
    }
    else
    {
        // Default to None, and grey out the virtual output with a hint
        midiOutputSelect.addNoneItem();
        midiOutputSelect.addMidiDevices(midiOutputDevices);
        midiOutputSelect.setDeviceUnavailable(VIRTUAL_MIDI_OUTPUT_DEVICE, VIRTUAL_MIDI_UNAVAILABLE_TEXT);
    }

    restoreMidiOutput(virtualOutputAvailable);
    midiOutputSelect.onChange = [this]
    {
        updateMidiOutput();
        saveMidiOutput();
    };
    
    addAndMakeVisible(scaleSelector);
    scaleSelector.onScaleTypeChange = [this] { updateScale(); };
    scaleSelector.onScaleRootChange = [this] { updateScale(); };

    midiKeyboardManager.onScaleChange = [this](ScaleRoot scaleRoot) { scaleSelector.setScaleRoot(scaleRoot); };

    // The output is already open (or None), so just send the starting scale
    updateScale();

    // Make sure you set the size of the component after
    // you add any child components.
    setSize (800, 250);
}

MainComponent::~MainComponent()
{
}

void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{
    const int SCALE_SELECTOR_HEIGHT = 100;
    const int MIDI_SELECT_HEIGHT = 25;
    const int MARGIN = 10;

    auto area = getLocalBounds().reduced(MARGIN);

    midiKeyboardSelect.setBounds(area.removeFromTop(MIDI_SELECT_HEIGHT));

    area.removeFromTop(MARGIN);

    midiOutputSelect.setBounds(area.removeFromTop(MIDI_SELECT_HEIGHT));

    area.removeFromTop(MARGIN);
    
    scaleSelector.setBounds(area.removeFromTop(SCALE_SELECTOR_HEIGHT));
}

void MainComponent::updateMidiOutput()
{
    midiOutputManager.setMidiOutput(midiOutputSelect.getSelectedDevice().get());
    updateScale();
}

//==============================================================================
// The rules are in DeviceChoices, which has tests. Restoring doesn't save, so a
// fallback for a missing device isn't remembered.

void MainComponent::restoreMidiKeyboard()
{
    if (auto device = DeviceChoices::keyboardToRestore(settings.get().midiKeyboard, juce::MidiInput::getAvailableDevices()))
    {
        midiKeyboardSelect.setSelectedDevice(*device, juce::dontSendNotification);
        midiKeyboardManager.setMidiInput(&*device);
    }
}

void MainComponent::restoreMidiOutput(bool virtualOutputAvailable)
{
    const auto device = DeviceChoices::outputToRestore(settings.get().midiOutput,
                                                       juce::MidiOutput::getAvailableDevices(),
                                                       VIRTUAL_MIDI_OUTPUT_DEVICE,
                                                       virtualOutputAvailable);

    if (device.has_value())
    {
        midiOutputSelect.setSelectedDevice(*device, juce::dontSendNotification);
    }

    midiOutputManager.setMidiOutput(device.has_value() ? &*device : nullptr);
}

void MainComponent::saveMidiKeyboard()
{
    const auto saved = DeviceChoices::toSave(midiKeyboardSelect.getSelectedDevice().get(), VIRTUAL_MIDI_OUTPUT_DEVICE);
    settings.update([&](AppSettings& s) { s.midiKeyboard = saved; });
}

void MainComponent::saveMidiOutput()
{
    const auto saved = DeviceChoices::toSave(midiOutputSelect.getSelectedDevice().get(), VIRTUAL_MIDI_OUTPUT_DEVICE);
    settings.update([&](AppSettings& s) { s.midiOutput = saved; });
}

void MainComponent::updateScale()
{
    midiOutputManager.sendScale(scaleSelector.getScale());
}

