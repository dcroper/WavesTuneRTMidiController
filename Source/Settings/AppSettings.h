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

//==============================================================================
/** A remembered MIDI device choice. */
struct SavedMidiDevice
{
    // Values for type
    static constexpr const char* unset = "unset";                 // never chosen
    static constexpr const char* none = "none";                   // "None" chosen
    static constexpr const char* device = "device";               // a device, by identifier and name
    static constexpr const char* virtualOutput = "virtual";       // the app's own virtual MIDI output

    juce::String type { unset };
    juce::String identifier;
    juce::String name;

    bool isType(const char* t) const { return type == t; }

    static SavedMidiDevice make(const char* t, const juce::MidiDeviceInfo& info = {})
    {
        SavedMidiDevice saved;
        saved.type = t;
        saved.identifier = info.identifier;
        saved.name = info.name;
        return saved;
    }

    bool operator==(const SavedMidiDevice& other) const
    {
        return type == other.type && identifier == other.identifier && name == other.name;
    }

    bool operator!=(const SavedMidiDevice& other) const { return ! operator==(other); }

    /**
        Finds this saved device among the available ones: by identifier first,
        since names can repeat, then by name, since identifiers can change (for
        example when a USB device moves to another port). Returns nothing unless
        type is "device".
    */
    std::optional<juce::MidiDeviceInfo> findIn(const juce::Array<juce::MidiDeviceInfo>& available) const
    {
        if (! isType(device))
        {
            return {};
        }

        if (identifier.isNotEmpty())
        {
            for (const auto& candidate : available)
            {
                if (candidate.identifier == identifier)
                {
                    return candidate;
                }
            }
        }

        for (const auto& candidate : available)
        {
            if (candidate.name == name)
            {
                return candidate;
            }
        }

        return {};
    }

    /** Writes this device as a JSON object. Keys in base that aren't known are
        kept, so a newer version's values survive.
    */
    juce::var toVar(const juce::var& base = {}) const;

    /** Reads a saved device. Missing or invalid values keep their defaults;
        anything unknown is skipped. Both are reported in warnings, prefixed
        with path (for example "midiKeyboard").
    */
    static SavedMidiDevice fromVar(const juce::var& value, const juce::String& path, juce::StringArray& warnings);
};

//==============================================================================
/**
    The settings file's schema version, written as "major.minor".

    - A new minor version only adds fields or field values. Any version of the
      app with the same major can read and write it: unknown fields and values
      are kept, and missing ones get their defaults.
    - A new major version is anything older apps can't handle, such as renaming
      a field or changing what a value means. An app can't read or write a
      newer major; it backs the file up and starts again (see SettingsStore).
      Newer apps migrate older majors (see AppSettings::migrateToCurrentMajor).
*/
struct SchemaVersion
{
    int major = 1;
    int minor = 0;

    /** Parses "major.minor". Returns nothing if the text isn't in that form. */
    static std::optional<SchemaVersion> parse(const juce::String& text);

    juce::String toString() const { return juce::String(major) + "." + juce::String(minor); }

    bool operator==(const SchemaVersion& other) const { return major == other.major && minor == other.minor; }
};

//==============================================================================
/**
    All settings the app remembers between runs, saved as JSON by SettingsStore.

    Adding a setting is a minor schema change: add a member with a default
    value, write it in toVar(), read it in fromVar() (and add its key to the
    known keys there), and increase currentSchema.minor.
*/
struct AppSettings
{
    /** The newest schema version this app reads and writes. */
    static constexpr SchemaVersion currentSchema { 1, 0 };

    SavedMidiDevice midiKeyboard;
    SavedMidiDevice midiOutput;

    /**
        Writes the settings as a JSON object for the given schema version.

        base is the file's JSON as it was loaded or last saved: its keys are
        kept, including ones this version doesn't know. previous is what the
        settings were when base was read. Only settings that differ from previous
        are written, so a value this version couldn't read (for example a field
        value added in a newer minor version) isn't replaced unless it was changed.
    */
    juce::var toVar(const juce::var& base = {}, const AppSettings* previous = nullptr,
                    SchemaVersion schema = currentSchema) const;

    /** Reads the settings from a JSON object of the current major version.
        Missing or invalid values keep their defaults; unknown keys are skipped.
        Both are reported in warnings.
    */
    static AppSettings fromVar(const juce::var& json, juce::StringArray& warnings);

    /** The file's schemaVersion. Returns 1.0 if it has none, and nothing if it
        isn't valid "major.minor" text.
    */
    static std::optional<SchemaVersion> readSchemaVersion(const juce::var& json);

    /** Converts settings JSON from an older major version to currentSchema's
        major. Returns nothing if fromMajor can't be migrated. Version 1 is the
        first major, so there's nothing to migrate yet.
    */
    static std::optional<juce::var> migrateToCurrentMajor(const juce::var& json, int fromMajor);
};
