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

#include "ToggleButtonGroup.h"

//==============================================================================
ToggleButtonGroup::ToggleButtonGroup()
  : deselectable(false),
    enabledButtonId(0)
{
}

ToggleButtonGroup::~ToggleButtonGroup()
{
}

int ToggleButtonGroup::getSelectedId() const
{
    if (enabledButtonId > 0)
    {
        return buttonList[enabledButtonId - 1]->isEnabled() ? enabledButtonId : 0;
    }
    else
    {
        return enabledButtonId;
    }
}

void ToggleButtonGroup::setSelectedId(int newButtonId, bool sendChange)
{
    if (newButtonId > 0 && newButtonId != enabledButtonId)
    {
        onClick(newButtonId, sendChange);
    }
    else if (newButtonId == 0 && enabledButtonId) // toggling off a button when newButtonId is 0
    {
        onClick(enabledButtonId, sendChange);
    }
}

void ToggleButtonGroup::addButtonList(const juce::StringArray& buttons)
{
    for (auto& button : buttons)
    {
        addButton(button);
    }
}

void ToggleButtonGroup::setButtonEnabled(int buttonId, bool shouldBeEnabled)
{
    buttonList[buttonId - 1]->setEnabled(shouldBeEnabled);
}

void ToggleButtonGroup::setDeselectable(bool shouldBeDeselectable)
{
    deselectable = shouldBeDeselectable;
}

int ToggleButtonGroup::getNumButtons() const
{
    return buttonList.size();
}

void ToggleButtonGroup::paint (juce::Graphics& g)
{
}

void ToggleButtonGroup::resized()
{
    if (!buttonList.isEmpty())
    {
        auto area = getLocalBounds();

        auto buttonWidth = area.getWidth() / size();
        for (auto& button : buttonList)
        {
            button->setBounds(area.removeFromLeft(buttonWidth));
        }
    }
}

int ToggleButtonGroup::size() const
{
    return buttonList.size();
}

void ToggleButtonGroup::addButton(const juce::String& name)
{
    std::shared_ptr<juce::TextButton> newButton(new juce::TextButton);
    newButton->setButtonText(name);
    newButton->setName(name);
    newButton->setLookAndFeel(&lookAndFeel);
    newButton->setToggleable(true);
    newButton->setRadioGroupId(1, juce::sendNotificationSync);
    int buttonId = buttonList.size() + 1;
    newButton->onClick = [this, buttonId] { onClick(buttonId, true); };

    if (!buttonList.isEmpty())
    {
        auto lastButton = buttonList.getLast();
        lastButton->setConnectedEdges(lastButton->getConnectedEdgeFlags() | juce::Button::ConnectedOnRight);
        newButton->setConnectedEdges(juce::Button::ConnectedOnLeft);
    }

    addAndMakeVisible(*newButton);
    buttonList.add(newButton);
}

void ToggleButtonGroup::onClick(int buttonId, bool sendChange)
{
    int prevEnabledButtonId = enabledButtonId;
    if (deselectable && prevEnabledButtonId == buttonId)
    {
        enabledButtonId = 0;
        buttonList[buttonId - 1]->setToggleState(false, false);
    }
    else
    {
        enabledButtonId = buttonId;
        buttonList[buttonId - 1]->setToggleState(true, false);
    }
    
    if (prevEnabledButtonId != enabledButtonId)
    {
        if (onChange)
        {
            onChange();
        }
    }
}
