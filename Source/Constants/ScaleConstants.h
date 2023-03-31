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

struct ScaleRoot
{
    enum Letter { A = 1, B = 2, C = 3, D = 4, E = 5, F = 6, G = 7 };
    enum Sign { natural = 0, flat = 1, sharp = 2 };

    static const juce::StringArray letterStrs;
    static const juce::StringArray signUniStrs;
    static const juce::StringArray signStrs;
    
    static Letter getLetter(const juce::StringRef letterStr);
    static Sign getSign(const juce::StringRef signStr);

    Letter letter;
    Sign sign;
};

struct Scale
{
    enum Type { chromatic = 1, major = 2, naturalMinor = 3 };

    static const juce::StringArray typeStrs;

    ScaleRoot root;
    Type type;
};
