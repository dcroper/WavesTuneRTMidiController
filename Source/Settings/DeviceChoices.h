/*
  ==============================================================================

    This file is part of WavesTuneRTMidiController.
    Copyright (C) 2026 David Roper

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
#include "AppSettings.h"

//==============================================================================
/**
    The rules for which MIDI devices are selected at startup, and what is saved
    when one is picked. Kept apart from the UI so they can be tested.

    Only choices made in the drop-downs are saved. A fallback made at startup,
    because a saved device is missing, isn't saved, so the device is selected
    again the next time it's connected.
*/
namespace DeviceChoices
{
    /** The keyboard to select at startup: the saved one if it's connected,
        otherwise nothing (None).
    */
    inline std::optional<juce::MidiDeviceInfo> keyboardToRestore(const SavedMidiDevice& saved,
                                                                  const juce::Array<juce::MidiDeviceInfo>& available)
    {
        return saved.findIn(available);
    }

    /** The output to select at startup:
        - the saved device, if it's connected
        - otherwise the virtual output, if it could be created. This also covers
          a saved "None", because None is only offered when the virtual output
          is unavailable.
        - otherwise nothing (None)
    */
    inline std::optional<juce::MidiDeviceInfo> outputToRestore(const SavedMidiDevice& saved,
                                                                const juce::Array<juce::MidiDeviceInfo>& available,
                                                                const juce::MidiDeviceInfo& virtualDevice,
                                                                bool virtualAvailable)
    {
        if (auto device = saved.findIn(available))
        {
            return device;
        }

        if (virtualAvailable)
        {
            return virtualDevice;
        }

        return {};
    }

    /** What to save when a device is picked in a drop-down. nullptr means None. */
    inline SavedMidiDevice toSave(const juce::MidiDeviceInfo* selected, const juce::MidiDeviceInfo& virtualDevice)
    {
        if (selected == nullptr)
        {
            return SavedMidiDevice::make(SavedMidiDevice::none);
        }

        if (*selected == virtualDevice)
        {
            return SavedMidiDevice::make(SavedMidiDevice::virtualOutput, *selected);
        }

        return SavedMidiDevice::make(SavedMidiDevice::device, *selected);
    }
}
