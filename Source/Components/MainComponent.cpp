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

//==============================================================================
static const juce::MidiDeviceInfo VIRTUAL_MIDI_OUTPUT_DEVICE("WavesTuneRTVirtualMIDI", "");

#if JUCE_WINDOWS
 // Windows needs the teVirtualMIDI driver, which comes with loopMIDI
 static const juce::String VIRTUAL_MIDI_UNAVAILABLE_TEXT("WavesTuneRTVirtualMIDI (install loopMIDI to enable)");
#else
 static const juce::String VIRTUAL_MIDI_UNAVAILABLE_TEXT("WavesTuneRTVirtualMIDI (unavailable)");
#endif

MainComponent::MainComponent()
{
    addAndMakeVisible(midiKeyboardSelect);
    midiKeyboardSelect.setLabelText("MIDI Keyboard:");
    midiKeyboardSelect.addNoneItem();
    midiKeyboardSelect.addMidiDevices(juce::MidiInput::getAvailableDevices());
    midiKeyboardSelect.onChange = [this] { midiKeyboardManager.setMidiInput(midiKeyboardSelect.getSelectedDevice().get()); };

    // Open the virtual output first, to find out whether it can be created
    midiOutputManager.setMidiOutput(&VIRTUAL_MIDI_OUTPUT_DEVICE);
    const bool virtualOutputAvailable = midiOutputManager.hasMidiOutput();

    addAndMakeVisible(midiOutputSelect);
    midiOutputSelect.setLabelText("MIDI Output:");
    juce::Array<juce::MidiDeviceInfo> midiOutputDevices;
    midiOutputDevices.add(VIRTUAL_MIDI_OUTPUT_DEVICE);
    midiOutputDevices.addArray(juce::MidiOutput::getAvailableDevices());

    if (virtualOutputAvailable)
    {
        midiOutputSelect.addMidiDevices(midiOutputDevices);
        midiOutputSelect.setSelectedDevice(VIRTUAL_MIDI_OUTPUT_DEVICE, juce::dontSendNotification);
    }
    else
    {
        // Default to None, and grey out the virtual output with a hint
        midiOutputSelect.addNoneItem();
        midiOutputSelect.addMidiDevices(midiOutputDevices);
        midiOutputSelect.setDeviceUnavailable(VIRTUAL_MIDI_OUTPUT_DEVICE, VIRTUAL_MIDI_UNAVAILABLE_TEXT);
    }

    midiOutputSelect.onChange = [this] { updateMidiOutput(); };
    
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

void MainComponent::updateScale()
{
    midiOutputManager.sendScale(scaleSelector.getScale());
}

