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

#include <JuceHeader.h>
#include "../Source/Settings/AppSettings.h"
#include "../Source/Settings/SettingsStore.h"

namespace
{
    // A folder in the temp directory that's deleted when the test section ends
    struct TemporaryFolder
    {
        TemporaryFolder()
            : folder(juce::File::getSpecialLocation(juce::File::tempDirectory)
                         .getNonexistentChildFile("WavesTuneSettingsTest", {}))
        {
        }

        ~TemporaryFolder() { folder.deleteRecursively(); }

        juce::File settingsFile() const { return folder.getChildFile("Settings").getChildFile("settings.json"); }

        void write(const juce::String& contents) const
        {
            settingsFile().getParentDirectory().createDirectory();
            settingsFile().replaceWithText(contents);
        }

        juce::File folder;
    };

    juce::MidiDeviceInfo keyboard() { return { "My Keyboard", "id-keyboard" }; }

    AppSettings exampleSettings()
    {
        AppSettings settings;
        settings.midiKeyboard = SavedMidiDevice::make(SavedMidiDevice::device, keyboard());
        settings.midiOutput = SavedMidiDevice::make(SavedMidiDevice::virtualOutput, { "WavesTuneRTVirtualMIDI", "" });
        return settings;
    }

    bool sameSettings(const AppSettings& a, const AppSettings& b)
    {
        return a.midiKeyboard == b.midiKeyboard && a.midiOutput == b.midiOutput;
    }

    AppSettings parse(const juce::String& json, juce::StringArray& warnings)
    {
        return AppSettings::fromVar(juce::JSON::parse(json), warnings);
    }
}

//==============================================================================
class AppSettingsTests : public juce::UnitTest
{
public:
    AppSettingsTests() : juce::UnitTest("AppSettings", "ControllerApp") {}

    void runTest() override
    {
        beginTest("Defaults are unset");
        {
            AppSettings settings;
            expectEquals(settings.midiKeyboard.type, juce::String(SavedMidiDevice::unset));
            expectEquals(settings.midiOutput.type, juce::String(SavedMidiDevice::unset));
        }

        beginTest("JSON round trip keeps every value, with no warnings");
        {
            juce::StringArray warnings;
            const auto loaded = parse(juce::JSON::toString(exampleSettings().toVar()), warnings);
            expect(sameSettings(loaded, exampleSettings()));
            expectEquals(warnings.size(), 0, warnings.joinIntoString("; "));
        }

        beginTest("JSON has the schema version and readable keys");
        {
            const auto json = juce::JSON::parse(juce::JSON::toString(exampleSettings().toVar()));
            expectEquals(json["schemaVersion"].toString(), AppSettings::currentSchema.toString());
            expectEquals(json["midiKeyboard"]["name"].toString(), juce::String("My Keyboard"));
            expectEquals(json["midiOutput"]["type"].toString(), juce::String(SavedMidiDevice::virtualOutput));
        }

        beginTest("A missing setting keeps its default, and the rest still load");
        {
            juce::StringArray warnings;
            const auto loaded = parse(R"({ "schemaVersion": "1.0", "midiKeyboard": { "type": "none" } })", warnings);
            expectEquals(loaded.midiKeyboard.type, juce::String(SavedMidiDevice::none));
            expectEquals(loaded.midiOutput.type, juce::String(SavedMidiDevice::unset));
            expectEquals(warnings.size(), 0, warnings.joinIntoString("; "));
        }

        beginTest("Unknown settings are ignored with a warning");
        {
            juce::StringArray warnings;
            const auto loaded = parse(R"({ "midiOutput": { "type": "device", "name": "Out", "colour": "red" }, "futureSetting": 3 })", warnings);
            expectEquals(loaded.midiOutput.name, juce::String("Out"));
            expectEquals(warnings.size(), 2, warnings.joinIntoString("; "));
            expect(warnings.joinIntoString("\n").contains("midiOutput.colour"));
            expect(warnings.joinIntoString("\n").contains("futureSetting"));
        }

        beginTest("Invalid values keep their defaults with a warning");
        {
            juce::StringArray warnings;
            const auto loaded = parse(R"({ "midiKeyboard": { "type": "keyboard", "name": 42 }, "midiOutput": "not an object" })", warnings);
            expectEquals(loaded.midiKeyboard.type, juce::String(SavedMidiDevice::unset));
            expectEquals(loaded.midiKeyboard.name, juce::String());
            expectEquals(loaded.midiOutput.type, juce::String(SavedMidiDevice::unset));
            expectEquals(warnings.size(), 3, warnings.joinIntoString("; "));
        }

        beginTest("Schema versions are read as major.minor");
        {
            const auto read = [](const char* json) { return AppSettings::readSchemaVersion(juce::JSON::parse(json)); };

            expect(read("{}") == SchemaVersion { 1, 0 }, "a file without schemaVersion should count as 1.0");
            expect(read(R"({ "schemaVersion": "1.3" })") == SchemaVersion { 1, 3 });
            expect(read(R"({ "schemaVersion": "12.40" })") == SchemaVersion { 12, 40 });

            for (const auto* invalid : { R"({ "schemaVersion": 1 })", R"({ "schemaVersion": "1" })",
                                         R"({ "schemaVersion": "1.x" })", R"({ "schemaVersion": "1.2.3" })",
                                         R"({ "schemaVersion": "-1.0" })" })
                expect(! read(invalid).has_value(), juce::String("accepted ") + invalid);
        }

        beginTest("Only changed settings are written over the file's values");
        {
            // The keyboard has a value this version doesn't know, and isn't changed
            const auto base = juce::JSON::parse(R"({ "schemaVersion": "1.0", "midiKeyboard": { "type": "bluetooth", "name": "BT" },
                                                     "midiOutput": { "type": "none" } })");
            juce::StringArray warnings;
            const auto previous = AppSettings::fromVar(base, warnings);

            auto changed = previous;
            changed.midiOutput = SavedMidiDevice::make(SavedMidiDevice::virtualOutput, { "WavesTuneRTVirtualMIDI", "" });

            const auto json = changed.toVar(base, &previous);
            expectEquals(json["midiKeyboard"]["type"].toString(), juce::String("bluetooth"), "an unchanged setting was rewritten");
            expectEquals(json["midiOutput"]["type"].toString(), juce::String(SavedMidiDevice::virtualOutput));
        }
    }
};

static AppSettingsTests appSettingsTests;

//==============================================================================
class SavedMidiDeviceTests : public juce::UnitTest
{
public:
    SavedMidiDeviceTests() : juce::UnitTest("SavedMidiDevice::findIn", "ControllerApp") {}

    void runTest() override
    {
        const juce::Array<juce::MidiDeviceInfo> available { juce::MidiDeviceInfo("Same Name", "id-1"),
                                                            juce::MidiDeviceInfo("Same Name", "id-2"),
                                                            juce::MidiDeviceInfo("Other", "id-3") };

        beginTest("Matches by identifier before name");
        {
            const auto found = SavedMidiDevice::make(SavedMidiDevice::device, { "Same Name", "id-2" }).findIn(available);
            expect(found.has_value());
            if (found.has_value())
                expectEquals(found->identifier, juce::String("id-2"));
        }

        beginTest("Falls back to name when the identifier changed");
        {
            const auto found = SavedMidiDevice::make(SavedMidiDevice::device, { "Other", "old-id" }).findIn(available);
            expect(found.has_value());
            if (found.has_value())
                expectEquals(found->identifier, juce::String("id-3"));
        }

        beginTest("Matches by name when no identifier was saved");
        {
            const auto found = SavedMidiDevice::make(SavedMidiDevice::device, { "Other", "" }).findIn(available);
            expect(found.has_value());
            if (found.has_value())
                expectEquals(found->identifier, juce::String("id-3"));
        }

        beginTest("Returns nothing when the device is missing");
        {
            expect(! SavedMidiDevice::make(SavedMidiDevice::device, { "Missing", "id-9" }).findIn(available).has_value());
        }

        beginTest("Returns nothing for unset, none and virtual");
        {
            for (const auto* type : { SavedMidiDevice::unset, SavedMidiDevice::none, SavedMidiDevice::virtualOutput })
            {
                expect(! SavedMidiDevice::make(type, { "Other", "id-3" }).findIn(available).has_value(),
                       juce::String("found a device for type ") + type);
            }
        }
    }
};

static SavedMidiDeviceTests savedMidiDeviceTests;

//==============================================================================
class SettingsStoreTests : public juce::UnitTest
{
public:
    SettingsStoreTests() : juce::UnitTest("SettingsStore", "ControllerApp") {}

    void runTest() override
    {
        beginTest("Missing file gives defaults and writes nothing");
        {
            TemporaryFolder temp;
            {
                SettingsStore store(temp.settingsFile());
                expectEquals(store.get().midiKeyboard.type, juce::String(SavedMidiDevice::unset));
            }
            expect(! temp.settingsFile().exists(), "a file was written without any change");
        }

        beginTest("saveNow writes the file, creating its folder");
        {
            TemporaryFolder temp;
            SettingsStore store(temp.settingsFile());
            store.update([](AppSettings& s) { s = exampleSettings(); });
            store.saveNow();

            expect(temp.settingsFile().existsAsFile());
            juce::StringArray warnings;
            expect(sameSettings(AppSettings::fromVar(juce::JSON::parse(temp.settingsFile()), warnings), exampleSettings()));
        }

        beginTest("The file uses the platform's line endings");
        {
            TemporaryFolder temp;
            SettingsStore store(temp.settingsFile());
            store.update([](AppSettings& s) { s = exampleSettings(); });
            store.saveNow();

            const auto text = temp.settingsFile().loadFileAsString();
            const auto lineCount = juce::StringArray::fromLines(text).size();
           #if JUCE_WINDOWS
            expectEquals(text.length() - text.removeCharacters("\r").length(), lineCount - 1, "not every line ends in CRLF");
           #else
            expect(! text.contains("\r"), "the file has CR characters");
           #endif
            expect(lineCount > 1, "the file has no line breaks");
        }

        beginTest("A failed save stays pending and is retried");
        {
            TemporaryFolder temp;

            // A folder where the file should be makes writing it fail
            temp.settingsFile().createDirectory();

            SettingsStore store(temp.settingsFile());
            store.update([](AppSettings& s) { s = exampleSettings(); });
            store.saveNow();
            expect(temp.settingsFile().isDirectory(), "the save unexpectedly replaced the folder");

            temp.settingsFile().deleteRecursively();
            store.saveNow();
            expect(temp.settingsFile().existsAsFile(), "the pending change wasn't retried");
        }

        beginTest("A change is saved when the store is destroyed");
        {
            TemporaryFolder temp;
            {
                SettingsStore store(temp.settingsFile());
                store.update([](AppSettings& s) { s = exampleSettings(); });
            }
            expect(temp.settingsFile().existsAsFile(), "the change was lost on quit");
        }

        beginTest("A new store loads what the last one saved");
        {
            TemporaryFolder temp;
            {
                SettingsStore store(temp.settingsFile());
                store.update([](AppSettings& s) { s = exampleSettings(); });
            }
            SettingsStore reloaded(temp.settingsFile());
            expect(sameSettings(reloaded.get(), exampleSettings()));
            expectEquals(reloaded.getLoadWarnings().size(), 0);
        }

        beginTest("A change is saved by the timer after the delay, not before");
        {
            TemporaryFolder temp;
            SettingsStore store(temp.settingsFile(), 100);
            store.update([](AppSettings& s) { s = exampleSettings(); });
            expect(! temp.settingsFile().exists(), "the file was written straight away");

            // Run the message loop so the timer can fire
            juce::MessageManager::getInstance()->runDispatchLoopUntil(1000);
            expect(temp.settingsFile().existsAsFile(), "the timer didn't save the change");
        }

        beginTest("An unreadable file is kept as .bak and defaults are used");
        {
            for (const auto* contents : { "{ not json", "[1, 2, 3]" })
            {
                TemporaryFolder temp;
                temp.write(contents);

                SettingsStore store(temp.settingsFile());
                const auto backup = temp.settingsFile().getSiblingFile("settings.json.bak");

                expectEquals(store.get().midiKeyboard.type, juce::String(SavedMidiDevice::unset));
                expect(! temp.settingsFile().exists(), "the unreadable file is still in place");
                expectEquals(backup.loadFileAsString(), juce::String(contents));
            }
        }

        beginTest("A file with unknown and missing values loads what it can");
        {
            TemporaryFolder temp;
            temp.write(R"({ "schemaVersion": "1.0", "midiKeyboard": { "type": "none" }, "futureSetting": true })");

            SettingsStore store(temp.settingsFile());
            expectEquals(store.get().midiKeyboard.type, juce::String(SavedMidiDevice::none));
            expectEquals(store.get().midiOutput.type, juce::String(SavedMidiDevice::unset));
            expectEquals(store.getLoadWarnings().size(), 1, store.getLoadWarnings().joinIntoString("; "));
        }

        beginTest("Saving keeps keys this version doesn't know");
        {
            TemporaryFolder temp;
            temp.write(R"({ "schemaVersion": "1.0", "futureSetting": { "a": [1, 2] },
                            "midiKeyboard": { "type": "none", "futureDeviceSetting": "x" } })");
            {
                SettingsStore store(temp.settingsFile());
                store.update([](AppSettings& s) { s = exampleSettings(); });
                store.saveNow();

                // A second save must keep them too
                store.update([](AppSettings& s) { s.midiOutput = SavedMidiDevice::make(SavedMidiDevice::none); });
            }

            const auto json = juce::JSON::parse(temp.settingsFile());
            expectEquals(juce::JSON::toString(json["futureSetting"], true), juce::String(R"({"a": [1, 2]})"));
            expectEquals(json["midiKeyboard"]["futureDeviceSetting"].toString(), juce::String("x"));
            expectEquals(json["midiKeyboard"]["name"].toString(), juce::String("My Keyboard"), "a known value wasn't updated");
            expectEquals(json["midiOutput"]["type"].toString(), juce::String(SavedMidiDevice::none), "the second change wasn't saved");
        }

        beginTest("A newer minor version is read, and its fields and version are kept");
        {
            TemporaryFolder temp;
            temp.write(R"({ "schemaVersion": "1.7", "newField": 5,
                            "midiKeyboard": { "type": "bluetooth", "name": "BT" }, "midiOutput": { "type": "none" } })");
            {
                SettingsStore store(temp.settingsFile());
                expect(store.getFileSchema() == SchemaVersion { 1, 7 });
                expectEquals(store.get().midiKeyboard.type, juce::String(SavedMidiDevice::unset), "an unknown type should load as unset");

                // Change only the output
                store.update([](AppSettings& s) { s.midiOutput = SavedMidiDevice::make(SavedMidiDevice::virtualOutput, { "WavesTuneRTVirtualMIDI", "" }); });
            }

            const auto json = juce::JSON::parse(temp.settingsFile());
            expectEquals(json["schemaVersion"].toString(), juce::String("1.7"), "the newer minor version was lowered");
            expectEquals((int) json["newField"], 5);
            expectEquals(json["midiKeyboard"]["type"].toString(), juce::String("bluetooth"), "the unchanged keyboard was rewritten");
            expectEquals(json["midiOutput"]["type"].toString(), juce::String(SavedMidiDevice::virtualOutput));
        }

        beginTest("A newer major version is backed up and a new file is written");
        {
            TemporaryFolder temp;
            const juce::String newer(R"({ "schemaVersion": "2.0", "renamedField": { "type": "none" } })");
            temp.write(newer);

            SettingsStore store(temp.settingsFile());
            const auto backup = temp.settingsFile().getSiblingFile("settings.json.bak");

            expectEquals(backup.loadFileAsString(), newer);
            expectEquals(store.get().midiKeyboard.type, juce::String(SavedMidiDevice::unset));

            const auto json = juce::JSON::parse(temp.settingsFile());
            expectEquals(json["schemaVersion"].toString(), AppSettings::currentSchema.toString(), "no new file in the current version");
            expect(! json.hasProperty("renamedField"), "the new file kept fields from the newer major");
        }

        beginTest("An older major that can't be converted is backed up and defaults are used");
        {
            TemporaryFolder temp;
            const juce::String older(R"({ "schemaVersion": "0.9", "midiKeyboard": { "type": "none" } })");
            temp.write(older);

            SettingsStore store(temp.settingsFile());
            expectEquals(store.get().midiKeyboard.type, juce::String(SavedMidiDevice::unset));
            expect(! temp.settingsFile().exists(), "the unconvertible file is still in place");
            expectEquals(temp.settingsFile().getSiblingFile("settings.json.bak").loadFileAsString(), older);
        }

        beginTest("An invalid schema version is backed up and defaults are used");
        {
            TemporaryFolder temp;
            temp.write(R"({ "schemaVersion": "one", "midiKeyboard": { "type": "none" } })");

            SettingsStore store(temp.settingsFile());
            expectEquals(store.get().midiKeyboard.type, juce::String(SavedMidiDevice::unset));
            expect(temp.settingsFile().getSiblingFile("settings.json.bak").existsAsFile());
        }
    }
};

static SettingsStoreTests settingsStoreTests;
