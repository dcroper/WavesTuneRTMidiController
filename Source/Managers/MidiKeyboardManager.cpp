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

#include "MidiKeyboardManager.h"

MidiKeyboardManager::MidiKeyboardManager()
  : midiListener(this)
{
}

MidiKeyboardManager::~MidiKeyboardManager()
{
}

void MidiKeyboardManager::setMidiInput(const juce::MidiDeviceInfo* const deviceInfo)
{
    if (midiDevice)
    {
        deviceManager.removeMidiInputDeviceCallback(midiDevice->identifier, &midiListener);
    }

    if (deviceInfo)
    {
        deviceManager.setMidiInputDeviceEnabled(deviceInfo->identifier, true);
        deviceManager.addMidiInputDeviceCallback(deviceInfo->identifier, &midiListener);
        midiDevice = std::make_unique<juce::MidiDeviceInfo>(*deviceInfo);
    }
    else
    {
        midiDevice.reset();
    }
}

MidiKeyboardManager::MidiListener::MidiListener(MidiKeyboardManager* manager)
  : manager(manager)
{
}

MidiKeyboardManager::MidiListener::~MidiListener()
{
}

void MidiKeyboardManager::MidiListener::handleIncomingMidiMessage(juce::MidiInput* source, const juce::MidiMessage& message)
{
    if (message.isNoteOn())
    {
        juce::String note = juce::MidiMessage::getMidiNoteName(message.getNoteNumber(), false, false, 0);
        ScaleRoot scaleRoot{};
        scaleRoot.letter = ScaleRoot::getLetter(note.substring(0, 1));
        scaleRoot.sign = note.length() > 1 ? ScaleRoot::getSign(note.substring(1, 2)) : ScaleRoot::natural;

        juce::MessageManager::callAsync([this, scaleRoot] { manager->onScaleChange(scaleRoot); });
    }
}
