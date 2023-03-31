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

#include <cmath>
#include "MidiOutputManager.h"
#include "../Components/ScaleSelector.h"

static const int DEFAULT_SCALE_TYPE_CHANNEL = 1;
static const int DEFAULT_SCALE_ROOT_CHANNEL = 2;
static const int DEFAULT_CC_START = 0;
static const int DEFAULT_RANGE = 6;

// scaleRootLetter goes from 1-7 starting at A, 8-14 are flats starting at Ab, 15-21 are sharps starting at A#. -1 means the value is invalid (ie. Cb)
static const juce::Array<int> SCALE_ROOT_MIDI_LOOKUP({-1, 13, 16, 0, 3, 6, 7, 10, 12, 15, -1, 2, 5, -1, 9, 14, -1, 1, 4, -1, 8, 11});

static const int SCALE_ROOT_MAX = 16;
static const int SCALE_TYPE_MAX = 43;
static const int MIDI_MAX_VALUE = 127;

MidiOutputManager::MidiOutputManager()
  : scaleTypeChannel(DEFAULT_SCALE_TYPE_CHANNEL),
    scaleTypeCCStart(DEFAULT_CC_START),
    scaleTypeCCRange(DEFAULT_RANGE),
    scaleRootChannel(DEFAULT_SCALE_ROOT_CHANNEL),
    scaleRootCCStart(DEFAULT_CC_START),
    scaleRootCCRange(DEFAULT_RANGE)
{
}

MidiOutputManager::~MidiOutputManager()
{
}

void MidiOutputManager::setMidiOutput(const juce::MidiDeviceInfo* const deviceInfo)
{
    if (deviceInfo != nullptr)
    {
        if (deviceInfo->identifier.isNotEmpty())
        {
            midiOutput = juce::MidiOutput::openDevice(deviceInfo->identifier);
        }
        else
        {
            midiOutput = juce::MidiOutput::createNewDevice(deviceInfo->name);
        }
    }
    else
    {
        midiOutput.reset();
    }
}

void MidiOutputManager::sendScale(const Scale& scale)
{
    if (scale.type > Scale::chromatic)
    {
        sendScaleType(scale.type);
        sendScaleRoot(scale.root.letter, scale.root.sign);
    }
    else
    {
        // hack to ensure updates happen when switching between chromatic and other scale types
        // since waves defaults back to A when switching to chromatic and doesn't update unless
        // the midi value changes.
        sendScaleRoot(ScaleRoot::A, ScaleRoot::natural);
        sendScaleType(scale.type);
    }
}

void MidiOutputManager::sendScaleRoot(const ScaleRoot::Letter& scaleRootLetter, const ScaleRoot::Sign& scaleRootSign)
{
    if (!midiOutput) return;

    static const float midiValPerScale = static_cast<float>(MIDI_MAX_VALUE) / SCALE_ROOT_MAX;
    int midiValue = midiValPerScale * SCALE_ROOT_MIDI_LOOKUP[scaleRootLetter + (7 * scaleRootSign)];

    juce::MidiBuffer midiBuffer;

    for (int i = 0; i < scaleRootCCRange; i++)
    {
        auto midiMessage = juce::MidiMessage::controllerEvent(scaleRootChannel, scaleRootCCStart + i, midiValue);
        midiBuffer.addEvent(midiMessage, i);
    }
    midiOutput->sendBlockOfMessagesNow(midiBuffer);
}

void MidiOutputManager::sendScaleType(const Scale::Type& scaleType)
{
    if (!midiOutput) return;

    static const float midiValPerType = static_cast<float>(MIDI_MAX_VALUE) / SCALE_TYPE_MAX;
    int midiValue = midiValPerType * scaleType;

    juce::MidiBuffer midiBuffer;

    for (int i = 0; i < scaleTypeCCRange; i++)
    {
        auto midiMessage = juce::MidiMessage::controllerEvent(scaleTypeChannel, scaleTypeCCStart + i, midiValue);
        midiBuffer.addEvent(midiMessage, i);
    }
    midiOutput->sendBlockOfMessagesNow(midiBuffer);
}
