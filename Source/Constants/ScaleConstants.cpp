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

#include "ScaleConstants.h"

const juce::StringArray ScaleRoot::letterStrs({ "", "A", "B", "C", "D", "E", "F", "G" });
const juce::StringArray ScaleRoot::signUniStrs({ L"", L"\u266D", L"\u266F" });
const juce::StringArray ScaleRoot::signStrs({ "", "b", "#" });
const juce::StringArray Scale::typeStrs({ "", "Chromatic", "Major", "Natural Minor" });

ScaleRoot::Letter ScaleRoot::getLetter(const juce::StringRef letterStr)
{
    return Letter(ScaleRoot::letterStrs.indexOf(letterStr));
}

ScaleRoot::Sign ScaleRoot::getSign(const juce::StringRef signStr)
{
    return Sign(ScaleRoot::signStrs.indexOf(signStr));
}
