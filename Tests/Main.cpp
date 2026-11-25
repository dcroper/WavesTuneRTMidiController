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

// Runs the app's unit tests (category "ControllerApp") and exits with 1 if any fail.
//
//   WavesTuneRTMidiControllerTests            run all tests
//   WavesTuneRTMidiControllerTests --list     list the test names
//   WavesTuneRTMidiControllerTests --name=X   run only the test named X

#include <JuceHeader.h>
#include <iostream>

namespace
{
    class ConsoleRunner : public juce::UnitTestRunner
    {
        void logMessage(const juce::String& message) override
        {
            std::cout << message << std::endl;
        }
    };

    const juce::String category("ControllerApp");
}

int main(int argc, char* argv[])
{
    // Gives the tests a message thread, which JUCE's Timer and some classes expect
    const juce::ScopedJuceInitialiser_GUI juceInitialiser;

    juce::ArgumentList args(argc, argv);

    if (args.containsOption("--list"))
    {
        for (auto* test : juce::UnitTest::getTestsInCategory(category))
        {
            std::cout << test->getName() << std::endl;
        }

        return 0;
    }

    ConsoleRunner runner;
    runner.setAssertOnFailure(false);

    if (args.containsOption("--name"))
    {
        runner.runTestsWithName(args.getValueForOption("--name"));
    }
    else
    {
        runner.runTestsInCategory(category);
    }

    int failures = 0;
    int passes = 0;

    for (int i = 0; i < runner.getNumResults(); ++i)
    {
        failures += runner.getResult(i)->failures;
        passes += runner.getResult(i)->passes;
    }

    std::cout << std::endl << std::string(65, '-') << std::endl;

    if (failures > 0)
    {
        std::cout << "Failed tests:" << std::endl;

        for (int i = 0; i < runner.getNumResults(); ++i)
        {
            const auto* result = runner.getResult(i);

            if (result->failures > 0)
            {
                std::cout << "  " << result->unitTestName << " / " << result->subcategoryName
                          << ": " << result->failures << " failure(s)" << std::endl;
            }
        }

        return 1;
    }

    std::cout << "All " << passes << " checks passed" << std::endl;
    return 0;
}
