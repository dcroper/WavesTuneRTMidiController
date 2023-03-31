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
#include "../Fonts/Fonts.h"

class ToggleButtonGroupLookAndFeel : public juce::LookAndFeel_V4
{
public:
    ToggleButtonGroupLookAndFeel()
    {
        setColour(juce::TextButton::buttonOnColourId, juce::Colour::fromRGB(209, 52, 206));
    }

    ~ToggleButtonGroupLookAndFeel() override {}

    juce::Font getTextButtonFont(juce::TextButton& button, int buttonHeight) override
    {
        return getGothicA1().withHeight(juce::jmin(40.0f, (float)buttonHeight * 0.6f));
    }

private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ToggleButtonGroupLookAndFeel);
};
