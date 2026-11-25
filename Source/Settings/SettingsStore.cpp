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

#include "SettingsStore.h"

//==============================================================================
// The platform's own line endings, so the file is easy to edit by hand
#if JUCE_WINDOWS
 static const char* const LINE_ENDINGS = "\r\n";
#else
 static const char* const LINE_ENDINGS = "\n";
#endif

SettingsStore::SettingsStore(juce::File fileToUse, int saveDelay)
  : file(std::move(fileToUse)),
    saveDelayMs(saveDelay)
{
    load();
}

SettingsStore::~SettingsStore()
{
    saveNow();
}

juce::File SettingsStore::getDefaultFile()
{
    auto folder = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory);

   #if JUCE_MAC
    // On macOS userApplicationDataDirectory is ~/Library
    folder = folder.getChildFile("Application Support");
   #endif

    return folder.getChildFile(ProjectInfo::projectName)
                 .getChildFile(juce::String(ProjectInfo::projectName) + ".json");
}

void SettingsStore::warn(const juce::String& message)
{
    loadWarnings.add(message);
    juce::Logger::writeToLog("Settings: " + message);
}

void SettingsStore::backUpAndUseDefaults(const juce::String& reason)
{
    // Keep the file instead of overwriting it, in case it's wanted later
    const auto backup = file.getSiblingFile(file.getFileName() + ".bak");
    warn(file.getFullPathName() + " " + reason + ", so it was moved to " + backup.getFileName()
         + " and defaults are used");
    file.moveFileTo(backup);
}

void SettingsStore::load()
{
    if (! file.existsAsFile())
    {
        return;
    }

    juce::var json;
    const auto parsed = juce::JSON::parse(file.loadFileAsString(), json);

    if (parsed.failed() || json.getDynamicObject() == nullptr)
    {
        backUpAndUseDefaults("isn't valid settings JSON");
        return;
    }

    const auto schema = AppSettings::readSchemaVersion(json);

    if (! schema.has_value())
    {
        backUpAndUseDefaults("has an invalid schemaVersion");
        return;
    }

    if (schema->major > AppSettings::currentSchema.major)
    {
        // This version can't read or write a newer major: start a new file now
        backUpAndUseDefaults("is from a newer version of the app (schema " + schema->toString() + ")");
        hasUnsavedChanges = true;
        saveNow();
        return;
    }

    const auto migrated = AppSettings::migrateToCurrentMajor(json, schema->major);

    if (! migrated.has_value())
    {
        backUpAndUseDefaults("uses schema " + schema->toString() + ", which can't be converted");
        return;
    }

    juce::StringArray warnings;
    settings = AppSettings::fromVar(*migrated, warnings);
    savedSettings = settings;
    savedJson = *migrated;

    // Keep a newer minor version: its extra fields are kept in the file
    if (schema->major == AppSettings::currentSchema.major && schema->minor > AppSettings::currentSchema.minor)
    {
        fileSchema = *schema;
    }

    for (const auto& w : warnings)
    {
        warn(w);
    }
}

void SettingsStore::update(const std::function<void(AppSettings&)>& change)
{
    change(settings);
    hasUnsavedChanges = true;
    startTimer(saveDelayMs);
}

void SettingsStore::timerCallback()
{
    saveNow();
}

void SettingsStore::saveNow()
{
    stopTimer();

    if (! hasUnsavedChanges)
    {
        return;
    }

    file.getParentDirectory().createDirectory();

    // Start from the file's JSON and write only what changed, so keys and values
    // this version doesn't know are kept
    const auto json = settings.toVar(savedJson, &savedSettings, fileSchema);

    // replaceWithText writes a temporary file first, so a crash can't leave a
    // half-written file
    if (file.replaceWithText(juce::JSON::toString(json), false, false, LINE_ENDINGS))
    {
        savedJson = json;
        savedSettings = settings;
        hasUnsavedChanges = false;
        return;
    }

    // Keep the change pending, so the next change or quitting tries again
    juce::Logger::writeToLog("Settings: couldn't save " + file.getFullPathName()
                             + ", trying again on the next change or when the app quits");
}
