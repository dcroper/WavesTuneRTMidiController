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

#include "AppSettings.h"

//==============================================================================
namespace
{
    const juce::Identifier schemaVersionKey("schemaVersion");
    const juce::Identifier midiKeyboardKey("midiKeyboard");
    const juce::Identifier midiOutputKey("midiOutput");

    const juce::Identifier typeKey("type");
    const juce::Identifier identifierKey("identifier");
    const juce::Identifier nameKey("name");

    // Reads a string property into target, if it's present and a string
    void readString(const juce::DynamicObject& object, const juce::Identifier& key,
                    const juce::String& path, juce::String& target, juce::StringArray& warnings)
    {
        if (! object.hasProperty(key))
        {
            return;
        }

        const auto& value = object.getProperty(key);

        if (! value.isString())
        {
            warnings.add(path + "." + key.toString() + " isn't text, so it was ignored");
            return;
        }

        target = value.toString();
    }

    // A copy of base to write into, keeping its properties, or a new object
    std::unique_ptr<juce::DynamicObject> copyOf(const juce::var& base)
    {
        if (auto* object = base.getDynamicObject())
        {
            return object->clone();
        }

        return std::make_unique<juce::DynamicObject>();
    }

    // Reports properties that aren't in known
    void warnAboutUnknownKeys(const juce::DynamicObject& object, std::initializer_list<juce::Identifier> known,
                              const juce::String& path, juce::StringArray& warnings)
    {
        for (const auto& property : object.getProperties())
        {
            if (std::find(known.begin(), known.end(), property.name) == known.end())
            {
                warnings.add("Unknown setting " + (path.isEmpty() ? juce::String() : path + ".")
                             + property.name.toString() + " isn't used by this version, but is kept in the file");
            }
        }
    }
}

//==============================================================================
juce::var SavedMidiDevice::toVar(const juce::var& base) const
{
    auto object = copyOf(base);
    object->setProperty(typeKey, type);
    object->setProperty(identifierKey, identifier);
    object->setProperty(nameKey, name);
    return object.release();
}

SavedMidiDevice SavedMidiDevice::fromVar(const juce::var& value, const juce::String& path, juce::StringArray& warnings)
{
    SavedMidiDevice saved;
    const auto* object = value.getDynamicObject();

    if (object == nullptr)
    {
        warnings.add(path + " isn't an object, so it was ignored");
        return saved;
    }

    juce::String type;
    readString(*object, typeKey, path, type, warnings);

    if (type.isNotEmpty())
    {
        if (type == unset || type == none || type == device || type == virtualOutput)
        {
            saved.type = type;
        }
        else
        {
            warnings.add(path + ".type \"" + type + "\" isn't known to this version, so " + path
                         + " is unset here, but kept in the file unless it's changed");
        }
    }

    readString(*object, identifierKey, path, saved.identifier, warnings);
    readString(*object, nameKey, path, saved.name, warnings);
    warnAboutUnknownKeys(*object, { typeKey, identifierKey, nameKey }, path, warnings);

    return saved;
}

//==============================================================================
std::optional<SchemaVersion> SchemaVersion::parse(const juce::String& text)
{
    const auto parts = juce::StringArray::fromTokens(text.trim(), ".", {});

    if (parts.size() != 2 || ! parts[0].containsOnly("0123456789") || ! parts[1].containsOnly("0123456789")
        || parts[0].isEmpty() || parts[1].isEmpty())
    {
        return {};
    }

    return SchemaVersion { parts[0].getIntValue(), parts[1].getIntValue() };
}

//==============================================================================
juce::var AppSettings::toVar(const juce::var& base, const AppSettings* previous, SchemaVersion schema) const
{
    auto object = copyOf(base);
    object->setProperty(schemaVersionKey, schema.toString());

    // Write a setting if it changed, or if the file doesn't have it yet
    const auto write = [&](const juce::Identifier& key, const SavedMidiDevice& current, const SavedMidiDevice* before)
    {
        if (before == nullptr || current != *before || ! object->hasProperty(key))
        {
            object->setProperty(key, current.toVar(base[key]));
        }
    };

    write(midiKeyboardKey, midiKeyboard, previous != nullptr ? &previous->midiKeyboard : nullptr);
    write(midiOutputKey, midiOutput, previous != nullptr ? &previous->midiOutput : nullptr);

    return object.release();
}

std::optional<SchemaVersion> AppSettings::readSchemaVersion(const juce::var& json)
{
    const auto* object = json.getDynamicObject();

    if (object == nullptr || ! object->hasProperty(schemaVersionKey))
    {
        return SchemaVersion {};
    }

    const auto& value = object->getProperty(schemaVersionKey);

    if (! value.isString())
    {
        return {};
    }

    return SchemaVersion::parse(value.toString());
}

std::optional<juce::var> AppSettings::migrateToCurrentMajor(const juce::var& json, int fromMajor)
{
    if (fromMajor == currentSchema.major)
    {
        return json;
    }

    // When the major version goes up, convert each older major here, one step at
    // a time (1 -> 2, then 2 -> 3, ...), so any older file can be brought up to date.
    return {};
}

AppSettings AppSettings::fromVar(const juce::var& json, juce::StringArray& warnings)
{
    AppSettings settings;
    const auto* object = json.getDynamicObject();

    if (object == nullptr)
    {
        warnings.add("The settings aren't a JSON object");
        return settings;
    }

    if (object->hasProperty(midiKeyboardKey))
    {
        settings.midiKeyboard = SavedMidiDevice::fromVar(object->getProperty(midiKeyboardKey), midiKeyboardKey.toString(), warnings);
    }

    if (object->hasProperty(midiOutputKey))
    {
        settings.midiOutput = SavedMidiDevice::fromVar(object->getProperty(midiOutputKey), midiOutputKey.toString(), warnings);
    }

    warnAboutUnknownKeys(*object, { schemaVersionKey, midiKeyboardKey, midiOutputKey }, {}, warnings);

    return settings;
}
