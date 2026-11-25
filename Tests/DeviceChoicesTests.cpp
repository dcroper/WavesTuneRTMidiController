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

#include <JuceHeader.h>
#include "../Source/Settings/DeviceChoices.h"

namespace
{
    const juce::MidiDeviceInfo virtualDevice("WavesTuneRTVirtualMIDI", "");
    const juce::MidiDeviceInfo outputA("Output A", "id-a");
    const juce::MidiDeviceInfo keyboardK("Keyboard K", "id-k");

    juce::String describe(const std::optional<juce::MidiDeviceInfo>& device)
    {
        return device.has_value() ? device->name : juce::String("None");
    }
}

//==============================================================================
class DeviceChoicesTests : public juce::UnitTest
{
public:
    DeviceChoicesTests() : juce::UnitTest("DeviceChoices", "ControllerApp") {}

    void runTest() override
    {
        const juce::Array<juce::MidiDeviceInfo> outputs { outputA };
        const juce::Array<juce::MidiDeviceInfo> noOutputs;

        const auto unset = SavedMidiDevice{};
        const auto none = SavedMidiDevice::make(SavedMidiDevice::none);
        const auto savedVirtual = SavedMidiDevice::make(SavedMidiDevice::virtualOutput, virtualDevice);
        const auto savedA = SavedMidiDevice::make(SavedMidiDevice::device, outputA);

        beginTest("Output: a connected saved device is selected");
        {
            expectEquals(describe(DeviceChoices::outputToRestore(savedA, outputs, virtualDevice, true)), outputA.name);
            expectEquals(describe(DeviceChoices::outputToRestore(savedA, outputs, virtualDevice, false)), outputA.name);
        }

        beginTest("Output: falls back to the virtual output when it's available");
        {
            for (const auto& saved : { unset, none, savedVirtual })
                expectEquals(describe(DeviceChoices::outputToRestore(saved, outputs, virtualDevice, true)), virtualDevice.name,
                             "saved type " + saved.type);

            expectEquals(describe(DeviceChoices::outputToRestore(savedA, noOutputs, virtualDevice, true)), virtualDevice.name,
                         "saved device missing");
        }

        beginTest("Output: falls back to None when the virtual output is unavailable");
        {
            for (const auto& saved : { unset, none, savedVirtual })
                expectEquals(describe(DeviceChoices::outputToRestore(saved, outputs, virtualDevice, false)), juce::String("None"),
                             "saved type " + saved.type);

            expectEquals(describe(DeviceChoices::outputToRestore(savedA, noOutputs, virtualDevice, false)), juce::String("None"),
                         "saved device missing");
        }

        beginTest("Keyboard: a connected saved device is selected, otherwise None");
        {
            const juce::Array<juce::MidiDeviceInfo> keyboards { keyboardK };
            const auto savedK = SavedMidiDevice::make(SavedMidiDevice::device, keyboardK);

            expectEquals(describe(DeviceChoices::keyboardToRestore(savedK, keyboards)), keyboardK.name);
            expectEquals(describe(DeviceChoices::keyboardToRestore(savedK, {})), juce::String("None"));
            expectEquals(describe(DeviceChoices::keyboardToRestore(unset, keyboards)), juce::String("None"));
            expectEquals(describe(DeviceChoices::keyboardToRestore(none, keyboards)), juce::String("None"));
        }

        beginTest("What's saved for a pick");
        {
            expectEquals(DeviceChoices::toSave(nullptr, virtualDevice).type, juce::String(SavedMidiDevice::none));
            expectEquals(DeviceChoices::toSave(&virtualDevice, virtualDevice).type, juce::String(SavedMidiDevice::virtualOutput));

            const auto saved = DeviceChoices::toSave(&outputA, virtualDevice);
            expectEquals(saved.type, juce::String(SavedMidiDevice::device));
            expectEquals(saved.identifier, outputA.identifier);
            expectEquals(saved.name, outputA.name);
        }
    }
};

static DeviceChoicesTests deviceChoicesTests;
