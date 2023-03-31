/*
  ==============================================================================

   This file is an addition to the JUCE Library to support virtual MIDI devices
   on Windows.
   Copyright (c) 2023 - David Roper

   The code included in this file is provided under the terms of the ISC license
   http://www.isc.org/downloads/software-support-policy/isc-license. Permission
   To use, copy, modify, and/or distribute this software for any purpose with or
   without fee is hereby granted provided that the above copyright notice and
   this permission notice appear in all copies.

   JUCE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
   DISCLAIMED.

  ==============================================================================
*/

#if __INTELLISENSE__
// makes life easier when developing using Visual Studio [Code]
#include <Windows.h>
#include "../juce_audio_devices.h"
#include "juce_win32_Midi.cpp"
#endif

namespace juce
{

typedef struct _VM_MIDI_PORT VM_MIDI_PORT, *LPVM_MIDI_PORT;

class TeVirtualMidiPort : public MidiOutput::Pimpl
{
public:
    enum Direction { rxOnly, txOnly, bidirectional };

    static MidiOutput::Pimpl* createOutputWrapper(const juce::String& name, Direction direction = bidirectional);

    ~TeVirtualMidiPort();

    String getDeviceIdentifier() override;
    String getDeviceName() override;

    void sendMessageNow(const MidiMessage& midiMessage) override;

private:
    TeVirtualMidiPort(const juce::String& name, Direction direction);

    LPVM_MIDI_PORT lpvmMidiPort;
    juce::String name;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TeVirtualMidiPort)
};

/* TE_VM_FLAGS_PARSE_RX tells the driver to always provide valid preparsed MIDI-commands either via Callback or via virtualMIDIGetData */
#define TE_VM_FLAGS_PARSE_RX (1)
/* TE_VM_FLAGS_PARSE_TX tells the driver to parse all data received via virtualMIDISendData */
#define TE_VM_FLAGS_PARSE_TX (2)
/* TE_VM_FLAGS_INSTANTIATE_RX_ONLY - Only the "midi-out" part of the port is created */
#define TE_VM_FLAGS_INSTANTIATE_RX_ONLY (4)
/* TE_VM_FLAGS_INSTANTIATE_TX_ONLY - Only the "midi-in" part of the port is created */
#define TE_VM_FLAGS_INSTANTIATE_TX_ONLY (8)

// Callback interface for midi input data
typedef void (CALLBACK* LPVM_MIDI_DATA_CB)(LPVM_MIDI_PORT midiPort, LPBYTE midiDataBytes, DWORD length, DWORD_PTR dwCallbackInstance);

// Creates a virtual midi port
typedef LPVM_MIDI_PORT(CALLBACK* LPVM_CREATE_PORT_EX2)(LPCWSTR portName, LPVM_MIDI_DATA_CB callback, DWORD_PTR dwCallbackInstance, DWORD maxSysexLength, DWORD flags);

// Closes a previously created virtual midi port
typedef void (CALLBACK* LPVM_CLOSE_PORT)(LPVM_MIDI_PORT midiPort);

// Sends midi data
typedef BOOL(CALLBACK* LPVM_SEND_DATA)(LPVM_MIDI_PORT midiPort, LPBYTE midiDataBytes, DWORD length);

static HINSTANCE hinstLib = LoadLibrary(TEXT("teVirtualMIDI.dll"));

class TeVirtualMidiDriver : public DeletedAtShutdown
{
public:
    ~TeVirtualMidiDriver()
    {
        clearSingletonInstance();
    }

    const LPVM_CREATE_PORT_EX2 virtualMIDICreatePortEx2;
    const LPVM_CLOSE_PORT virtualMIDIClosePort;
    const LPVM_SEND_DATA virtualMIDISendData;

    static TeVirtualMidiDriver* getDriver()
    {
        TeVirtualMidiDriver* driver = nullptr;
        if (hinstLib != nullptr)
        {
            driver = getInstance();
        }
        return driver;
    }

private:
    TeVirtualMidiDriver()
      : virtualMIDICreatePortEx2((LPVM_CREATE_PORT_EX2) GetProcAddress(hinstLib, "virtualMIDICreatePortEx2")),
        virtualMIDIClosePort((LPVM_CLOSE_PORT) GetProcAddress(hinstLib, "virtualMIDIClosePort")),
        virtualMIDISendData((LPVM_SEND_DATA) GetProcAddress(hinstLib, "virtualMIDISendData"))
    {
    }

    JUCE_DECLARE_SINGLETON(TeVirtualMidiDriver, false)
};

JUCE_IMPLEMENT_SINGLETON(TeVirtualMidiDriver)

TeVirtualMidiPort::TeVirtualMidiPort(const juce::String& name, Direction direction)
  : name(name)
{
    DWORD directionFlag;
    switch (direction)
    {
    case txOnly: directionFlag = TE_VM_FLAGS_INSTANTIATE_TX_ONLY; break;
    case rxOnly: directionFlag = TE_VM_FLAGS_INSTANTIATE_RX_ONLY; break;
    default: directionFlag = 0;
    }
    DWORD flags = TE_VM_FLAGS_PARSE_RX | directionFlag;
    lpvmMidiPort = TeVirtualMidiDriver::getDriver()->virtualMIDICreatePortEx2(name.toWideCharPointer(), NULL, NULL, 0, flags);
}

TeVirtualMidiPort::~TeVirtualMidiPort()
{
    TeVirtualMidiDriver::getDriver()->virtualMIDIClosePort(lpvmMidiPort);
}

String TeVirtualMidiPort::getDeviceIdentifier()
{
    return String();
}

String TeVirtualMidiPort::getDeviceName()
{
    return String();
}

void TeVirtualMidiPort::sendMessageNow(const MidiMessage& midiMessage)
{
    TeVirtualMidiDriver::getDriver()->virtualMIDISendData(lpvmMidiPort, (LPBYTE) midiMessage.getRawData(), midiMessage.getRawDataSize());
}

MidiOutput::Pimpl* TeVirtualMidiPort::createOutputWrapper(const juce::String& name, Direction direction)
{
    return new TeVirtualMidiPort(name, direction);
}

std::unique_ptr<MidiOutput> MidiOutput::createNewDevice(const String& deviceName)
{
    if (deviceName.isEmpty())
        return {};

    std::unique_ptr<Pimpl> wrapper;

    try
    {
        wrapper.reset(TeVirtualMidiPort::createOutputWrapper(deviceName, TeVirtualMidiPort::txOnly));
    }
    catch (std::runtime_error&)
    {
        return {};
    }

    std::unique_ptr<MidiOutput> out;
    out.reset(new MidiOutput(wrapper->getDeviceName(), wrapper->getDeviceIdentifier()));

    out->internal = std::move(wrapper);

    return out;
}

} // namespace juce
