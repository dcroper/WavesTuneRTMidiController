# Changelog

The Release workflow uses each section below as the GitHub release notes for that version, so links are full URLs.

## Unreleased

### New
- The app remembers the selected MIDI Keyboard and MIDI Output, and selects them again on the next start. See [Settings](https://github.com/dcroper/WavesTuneRTMidiController#settings).

### Changed
- The `WavesTuneRTVirtualMIDI` output now stays available the whole time the app is running. Before, choosing another MIDI Output removed it until you switched back, so your DAW lost the connection.

## v1.0.0

### New
- **macOS support.** A universal app that runs natively on Apple silicon and Intel Macs, on macOS 12 or later. The `WavesTuneRTVirtualMIDI` output works without any extra software.
- Ready-made downloads for macOS and Windows on every release.

### Fixed
- Playing a note on the MIDI keyboard could briefly send the wrong scale root before the right one. It now sends the new scale root once.
- On Windows without loopMIDI or rtpMIDI, the app crashed at startup. It now starts normally: `WavesTuneRTVirtualMIDI` is greyed out with a hint to install loopMIDI, and the MIDI Output is set to None.
- The MIDI Keyboard can now be set back to None.

### Changed
- Updated to JUCE 9.0.6 (from 7.0.5).
- Now licensed under the [AGPLv3](https://www.gnu.org/licenses/agpl-3.0.en.html) (previously GPLv3), to match JUCE.

### Installing
The apps are not code-signed, so macOS and Windows show a warning the first time you open a new version. See [Installing](https://github.com/dcroper/WavesTuneRTMidiController#installing) for how to open them. On Windows, the virtual MIDI output still needs [loopMIDI](https://www.tobias-erichsen.de/software/loopmidi.html) or [rtpMIDI](https://www.tobias-erichsen.de/software/rtpmidi.html) installed.

## v0.1.0
First release, for Windows.
