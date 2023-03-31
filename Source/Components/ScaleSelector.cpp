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

#include "ScaleSelector.h"

//==============================================================================
ScaleSelector::ScaleSelector()
{
    addAndMakeVisible(scaleRootLetterButtons);
    juce::StringArray letterButtons(ScaleRoot::letterStrs);
    letterButtons.remove(0);
    scaleRootLetterButtons.addButtonList(letterButtons);
    scaleRootLetterButtons.onChange = [this] { updateEnabledScaleRootButtons(); onScaleRootChangeHelper(); };

    addAndMakeVisible(scaleRootSignButtons);
    juce::StringArray signButtons(ScaleRoot::signUniStrs);
    signButtons.remove(0);
    scaleRootSignButtons.addButtonList(signButtons);
    scaleRootSignButtons.onChange = [this] { onScaleRootChangeHelper(); };
    scaleRootSignButtons.setDeselectable(true);

    addAndMakeVisible(scaleTypeBox);
    juce::StringArray typeItems(Scale::typeStrs);
    typeItems.remove(0);
    scaleTypeBox.addItemList(typeItems, 1);
    scaleTypeBox.onChange = [this] { onScaleTypeChangeHelper(); };
    scaleTypeBox.setSelectedId(1, juce::sendNotificationSync);
    
    addAndMakeVisible(scaleLabel);
    scaleLabel.setText("SCALE", juce::dontSendNotification);
    scaleLabel.setColour(juce::Label::textColourId, juce::Colours::black);
}

ScaleSelector::~ScaleSelector()
{
}

Scale ScaleSelector::getScale() const
{
    Scale scale{};
    scale.root.letter = static_cast<ScaleRoot::Letter>(scaleRootLetterButtons.getSelectedId());
    scale.root.sign = scaleRootSignButtons.isEnabled() ? static_cast<ScaleRoot::Sign>(scaleRootSignButtons.getSelectedId()) : ScaleRoot::natural;
    scale.type = static_cast<Scale::Type>(scaleTypeBox.getSelectedId());
    return scale;
}

void ScaleSelector::setScaleRoot(const ScaleRoot& scaleRoot)
{
    scaleRootSignButtons.setSelectedId(scaleRoot.sign, false);
    scaleRootLetterButtons.setSelectedId(scaleRoot.letter, false);
    onScaleRootChangeHelper();
}

void ScaleSelector::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.fillAll (juce::Colours::silver);   // clear the background

    g.setColour (juce::Colours::black);
    g.drawRect (getLocalBounds(), 2);   // draw an outline around the component
}

void ScaleSelector::resized()
{
    const int MARGIN = 10;
    const int ROWS = 2;
    const int SCALE_LABEL_WIDTH = 70;

    auto area = getLocalBounds().reduced(MARGIN);

    auto firstRow = area.removeFromTop((area.getHeight() / ROWS) - (MARGIN / 2));
    area.removeFromTop(MARGIN);

    scaleRootSignButtons.setBounds(firstRow.removeFromRight((firstRow.getWidth() / scaleRootLetterButtons.getNumButtons()) * 2));
    scaleLabel.setBounds(firstRow.removeFromLeft(SCALE_LABEL_WIDTH));
    scaleTypeBox.setBounds(firstRow.reduced(MARGIN, 0));

    scaleRootLetterButtons.setBounds(area);
}

void ScaleSelector::onScaleTypeChangeHelper()
{
    if (scaleTypeBox.getSelectedId() == 1)
    {
        scaleRootLetterButtons.setEnabled(false);
        scaleRootSignButtons.setEnabled(false);
    }
    else
    {
        scaleRootLetterButtons.setEnabled(true);
        scaleRootSignButtons.setEnabled(true);
        updateEnabledScaleRootButtons();
    }

    if (onScaleTypeChange)
    {
        onScaleTypeChange();
    }
}

void ScaleSelector::updateEnabledScaleRootButtons()
{
    switch (scaleRootLetterButtons.getSelectedId())
    {
    case ScaleRoot::B:  scaleRootSignButtons.setButtonEnabled(ScaleRoot::flat, true);
                        scaleRootSignButtons.setButtonEnabled(ScaleRoot::sharp, false); break;
    case ScaleRoot::C:  scaleRootSignButtons.setButtonEnabled(ScaleRoot::flat, false);
                        scaleRootSignButtons.setButtonEnabled(ScaleRoot::sharp, true); break;
    case ScaleRoot::E:  scaleRootSignButtons.setButtonEnabled(ScaleRoot::flat, true);
                        scaleRootSignButtons.setButtonEnabled(ScaleRoot::sharp, false); break;
    case ScaleRoot::F:  scaleRootSignButtons.setButtonEnabled(ScaleRoot::flat, false);
                        scaleRootSignButtons.setButtonEnabled(ScaleRoot::sharp, true); break;
    default:            scaleRootSignButtons.setButtonEnabled(ScaleRoot::flat, true);
                        scaleRootSignButtons.setButtonEnabled(ScaleRoot::sharp, true);
    }
}

void ScaleSelector::onScaleRootChangeHelper()
{
    if (onScaleRootChange)
    {
        onScaleRootChange();
    }
}
