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
#include "../Constants/ScaleConstants.h"
#include "ToggleButtonGroup.h"

//==============================================================================
class ScaleSelector  : public juce::Component
{
public:
    ScaleSelector();
    ~ScaleSelector() override;

    std::function<void()> onScaleRootChange;
    std::function<void()> onScaleTypeChange;

    Scale getScale() const;
    void setScaleRoot(const ScaleRoot& scaleRoot);

    void paint (juce::Graphics&) override;
    void resized() override;

private:

    void onScaleTypeChangeHelper();
    void updateEnabledScaleRootButtons();
    void onScaleRootChangeHelper();

    ToggleButtonGroup scaleRootLetterButtons;
    ToggleButtonGroup scaleRootSignButtons;
    juce::ComboBox scaleTypeBox;
    juce::Label scaleLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ScaleSelector)
};
