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
#include "ToggleButtonGroupLookAndFeel.h"

//==============================================================================
class ToggleButtonGroup  : public juce::Component
{
public:
    ToggleButtonGroup();
    ~ToggleButtonGroup() override;

    std::function<void()> onChange;

    int getSelectedId() const;
    void setSelectedId(int newButtonId, bool sendChange);

    void addButtonList(const juce::StringArray& buttons);
    void setButtonEnabled(int buttonId, bool shouldBeEnabled);
    void setDeselectable(bool shouldBeDeselectable);

    int getNumButtons() const;

    void paint(juce::Graphics&) override;
    void resized() override;

private:

    void addButton(const juce::String& name);
    int size() const;

    void onClick(int buttonId, bool sendChange);

    ToggleButtonGroupLookAndFeel lookAndFeel;
    juce::Array<std::shared_ptr<juce::TextButton>> buttonList;
    int enabledButtonId;
    bool deselectable;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ToggleButtonGroup)
};
