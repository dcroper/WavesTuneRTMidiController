# WavesTuneRTMidiController
This application was created to simultaniously control the `Scale Root` and `Scale Type` parameters of multiple instances of the "Waves Tune Real-Time" plugin within [Studio One 6](https://www.presonus.com/products/Studio-One/new-in-studio-one-6). The plugins can be controlled using the user interface as well as an external MIDI keyboard. Even though Studio One was the DAW targetted when making this application, it could still work with other DAWs that support plugin parameter control via MIDI.

Studio One 6 has the following limitations that necessitated the need for this application:
1. No support for grouping multiple plugin parameters together across tracks such that they can all be controlled together.
1. Cannot globally connect multiple plugin parameters to the same MIDI control such that changing the MIDI control changes all of the parameters associated with it.
1. Cannot use MIDI notes to update plugin parameters.

This application's UI mirrors the `Scale` section of "Waves Tune Real-Time" (v14) plugin and allows you to control up 6 instances of the plugin by outputing 6 copies of the MIDI control value for both the `Scale Root` and `Scale Type` that is selected. These MIDI controls can then be assigned to the `Scale Root` and `Scale Type` parameters of the various plugin instances within Studio One.

![Screenshot](screenshot.png)

Additionally, you can select a MIDI keyboard and use the keyboard notes to control the `Scale Root` value. Pressing a note on the MIDI keyboard will set the `Scale Root` to corresponding value for the note played regardless of octive. You cannot control the `Scale Type` value with the MIDI keyboard.

## Virtual MIDI Output Device
This program creates a virtual MIDI device to allow direct communication with a DAW running on the same machine. To use this virtual MIDI device, `WavesTuneRTVirtualMIDI` must be selected as the MIDI Output.

- **macOS:** the virtual device is created with CoreMIDI. Nothing extra needs to be installed.
- **Windows:** the virtual device is created with the [teVirtualMIDI](https://www.tobias-erichsen.de/software/virtualmidi.html) driver, using the [juce-tevirtualmidi](https://github.com/dcroper/juce-tevirtualmidi) patch to JUCE. The driver must be installed first (see below). Without it, the virtual device can't be created, but you can still choose any other MIDI output.

## Installing
Download the zip for your platform from the [Releases](https://github.com/dcroper/WavesTuneRTMidiController/releases) page.

The apps are not code-signed, so macOS and Windows warn the first time you open a new version. The steps below get past the warning. You only need to do this once per version.

### macOS
These steps are for macOS 15 Sequoia and later. The app runs natively on both Apple silicon and Intel Macs.

1. Double-click `WavesTuneRTMidiController-<version>-macOS.zip` to unzip it.
1. Drag `WavesTuneRTMidiController.app` into your **Applications** folder.
1. Open the app. macOS says it can't verify the app. Click **Done**.
1. Open **System Settings > Privacy & Security**, scroll down to **Security**, and click **Open Anyway** next to the message about WavesTuneRTMidiController.
1. Click **Open Anyway** again and enter your password. The app opens, and later launches work normally.

Alternatively, instead of steps 3 to 5, run this in Terminal once after copying the app:
```
xattr -dr com.apple.quarantine /Applications/WavesTuneRTMidiController.app
```

### Windows
1. Install the teVirtualMIDI driver by installing either [loopMIDI](https://www.tobias-erichsen.de/software/loopmidi.html) or [rtpMIDI](https://www.tobias-erichsen.de/software/rtpmidi.html). You don't need to create any ports in them.
1. Right-click `WavesTuneRTMidiController-<version>-Windows-x64.zip`, choose **Extract All**, and move `WavesTuneRTMidiController.exe` anywhere you like.
1. Run it. If Windows shows **Windows protected your PC**, click **More info**, then **Run anyway**.
1. In your DAW, enable `WavesTuneRTVirtualMIDI` as a MIDI input.

## Building
To build the app yourself, or to publish a release, see [DEVELOPMENT.md](DEVELOPMENT.md).

## License
WavesTuneRTMidiController is licensed under the [GNU Affero General Public License v3.0](LICENSE) (AGPLv3).

This application uses the [JUCE Framework](https://juce.com), whose modules are used here under the terms of the [AGPLv3](https://www.gnu.org/licenses/agpl-3.0.en.html). JUCE is also available under a commercial licence from Raw Material Software Limited.

The [juce-tevirtualmidi](https://github.com/dcroper/juce-tevirtualmidi) patch adds a file to JUCE that is provided under the ISC license.
