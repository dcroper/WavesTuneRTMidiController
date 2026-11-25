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
    Loads AppSettings from a JSON file at startup and saves them shortly after
    each change, and again when the app quits.

    The file is WavesTuneRTMidiController.json in:
    - macOS: ~/Library/Application Support/WavesTuneRTMidiController/
    - Windows: %APPDATA%\WavesTuneRTMidiController (folder)
    - Linux: ~/.config/WavesTuneRTMidiController/
*/
class SettingsStore : private juce::Timer
{
public:
    /** Loads the settings from file, by default getDefaultFile(). Changes are
        saved saveDelayMs after the last one.
    */
    explicit SettingsStore(juce::File fileToUse = getDefaultFile(), int saveDelayMs = 1000);
    ~SettingsStore() override;

    const AppSettings& get() const { return settings; }

    /** Changes the settings, then saves them shortly after. */
    void update(const std::function<void(AppSettings&)>& change);

    /** Writes any pending change to disk now. */
    void saveNow();

    const juce::File& getFile() const { return file; }

    /** The schema version the file is written in: the app's own, or a newer
        minor version of the same major if the file already had one.
    */
    const SchemaVersion& getFileSchema() const { return fileSchema; }

    /** Problems found while loading: missing, invalid or unknown values. Each is
        also written to the log.
    */
    const juce::StringArray& getLoadWarnings() const { return loadWarnings; }

    /** The settings file in the user's application data folder. */
    static juce::File getDefaultFile();

private:
    void timerCallback() override;
    void load();

    void warn(const juce::String& message);
    void backUpAndUseDefaults(const juce::String& reason);

    juce::File file;
    int saveDelayMs;
    AppSettings settings;
    bool hasUnsavedChanges = false;
    juce::StringArray loadWarnings;

    // The file as last loaded or saved, so unknown keys and unchanged values are kept
    juce::var savedJson;
    AppSettings savedSettings;
    SchemaVersion fileSchema = AppSettings::currentSchema;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingsStore)
};
