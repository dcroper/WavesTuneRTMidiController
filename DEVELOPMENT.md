# Developing WavesTuneRTMidiController

How to build the app, test builds on GitHub, and publish releases. For what the app does and how to install it, see the [README](README.md).

The project uses [CMake](https://cmake.org) 3.22 or later and [JUCE](https://github.com/juce-framework/JUCE) 9.0.6. CMake downloads JUCE and the [juce-tevirtualmidi](https://github.com/dcroper/juce-tevirtualmidi) patch, and applies the patch, the first time you configure. This needs git and an internet connection.

## Building on macOS
Requires Xcode and CMake (for example `brew install cmake`). Build a universal (Apple silicon + Intel) Release app:
```
cmake -B build -G Xcode
cmake --build build --config Release
```
The app is written to `build/WavesTuneRTMidiController_artefacts/Release/WavesTuneRTMidiController.app`. Open `build/WavesTuneRTMidiController.xcodeproj` to work in Xcode.

## Building on Windows
JUCE needs Microsoft's MSVC compiler; MinGW is not supported. You can work in VS Code with the free Build Tools, or in the full Visual Studio. Either way, the app is written to `build\WavesTuneRTMidiController_artefacts\Release\WavesTuneRTMidiController.exe`.

### VS Code
1. Install [Build Tools for Visual Studio 2026](https://visualstudio.microsoft.com/downloads/) (under **Tools for Visual Studio**) and select the **Desktop development with C++** workload. This installs the MSVC compiler, the Windows SDK, CMake and Ninja. VS Code extensions don't include a compiler, so this step is required.
1. Install [Git](https://git-scm.com/download/win) and [VS Code](https://code.visualstudio.com/).
1. In VS Code, install the **C/C++** and **CMake Tools** extensions from Microsoft.
1. Open this repository's folder in VS Code.
1. When CMake Tools asks for a kit, choose **Visual Studio Build Tools 2026 Release - amd64**. To change it later, run **CMake: Select a Kit** from the Command Palette (`Ctrl+Shift+P`).
1. Run **CMake: Select Variant** and choose **Release**, or **Debug** while developing.
1. Click **Build** in the status bar (or press `F7`). The first build takes a while, because it downloads and builds JUCE.
1. To run or debug the app, run **CMake: Debug** (`Ctrl+F5`). It uses the MSVC debugger from the C/C++ extension.

### Visual Studio 2026
Install [Visual Studio 2026](https://visualstudio.microsoft.com/downloads/) with the **Desktop development with C++** workload, then either:
- Open this repository's folder with **File > Open > Folder**. Visual Studio reads `CMakeLists.txt` directly.
- Or generate a solution from a **Developer PowerShell for VS 2026**, then open the `WavesTuneRTMidiController` solution file in `build`:
  ```
  cmake -B build -G "Visual Studio 18 2026" -A x64
  cmake --build build --config Release
  ```
  The `Visual Studio 18 2026` generator needs CMake 4.2 or later. Run `cmake --version` to check.

Visual Studio 2022 works too, with the `Visual Studio 17 2022` generator.

Build Tools and Visual Studio Community are free for open-source projects like this one.

## Running the unit tests
The unit tests use JUCE's `UnitTest` and live in `Tests/`. They currently cover the settings code: `AppSettings` JSON, `SavedMidiDevice::findIn`, `DeviceChoices` and `SettingsStore`. They're built along with the app, and the Build workflow runs them on macOS and Windows.

```
ctest --test-dir build -C Release --output-on-failure
```

The test program can also be run directly. `--list` shows the test names, and `--name=<test>` runs just one:
```
build/WavesTuneRTMidiControllerTests_artefacts/Release/WavesTuneRTMidiControllerTests --name=SettingsStore
```

To add tests, add a `juce::UnitTest` subclass in the `"ControllerApp"` category to a file in `Tests/`, and list that file and any app sources it needs under `WavesTuneRTMidiControllerTests` in `CMakeLists.txt`. Use `expectEquals` where possible, so a failure shows the actual and expected values. To skip building the tests, configure with `-DWAVES_BUILD_TESTS=OFF`.

## Testing a build on GitHub
The [Build workflow](.github/workflows/build.yml) builds the macOS and Windows apps without making a release. It only runs when you start it, or on pull requests that have the `build` label.

1. On the **Actions** tab, choose **Build**, then **Run workflow**.
1. Pick the branch to build and click **Run workflow**.
1. When the run finishes, download the zips from the **Artifacts** section of the run's page. GitHub keeps them for 90 days.

To build a pull request, add the `build` label to it (create the label once under **Issues > Labels** first). The build shows in the pull request's **Checks**, and its zips are named `pr-<number>`. While the label is on, every new push to the pull request builds again automatically, and cancels a build that's still running. Remove the label to stop. Only people with triage access or higher can add labels, so outside contributors can't start builds. Once you add the label to their pull request, though, their later pushes build automatically.

## Publishing a release
Pushing a version tag starts the [Release workflow](.github/workflows/release.yml). It builds the macOS and Windows apps with the same Build workflow and puts them in a **draft** release, which you publish by hand. Drafts are used so releases work with GitHub's immutable releases, which lock a release's files once it's published.

1. Update `VERSION` in the `project()` line of `CMakeLists.txt`. `VERSION` is numbers and dots only, such as `1.1.0`.
1. In [CHANGELOG.md](CHANGELOG.md), rename `## Unreleased` to `## v1.1.0`, or add a `## v1.1.0` section at the top, written for people using the app. `git log --format='- %s' <previous tag>..HEAD` lists the commits to start from. Use full URLs for links, because the workflow uses the section as the release notes.
1. Commit, merge to `main`, then tag the commit and push the tag:
   ```
   git tag -a v1.1.0 -m "v1.1.0"
   git push origin v1.1.0
   ```
1. The workflow checks that the tag matches `VERSION`, that CHANGELOG.md has a section for it, and that the release isn't already published. If any check fails, it stops before building and nothing is created.
1. After about 10 minutes, a draft release appears on the **Releases** page, with the CHANGELOG.md section as its notes and `WavesTuneRTMidiController-<tag>-macOS.zip` and `WavesTuneRTMidiController-<tag>-Windows-x64.zip` attached. Watch its progress on the **Actions** tab.
1. Check the draft, then click **Edit** and **Publish release**.

For a pre-release, tag with a suffix such as `v1.1.0-rc1`. The draft is marked as a pre-release and uses the `## v1.1.0` section. When it's good, tag the same commit as `v1.1.0` for the final release.

To rebuild a draft, for example after fixing the workflow, run the **Release** workflow from the **Actions** tab and enter the tag. It replaces the earlier draft. A published release can't be rebuilt; push a new tag instead.

To make published releases unchangeable, turn on immutable releases in the repository's **Settings → General → Releases**.

## Updating JUCE or the patch
`CMakeLists.txt` pins both:

| Setting | Pins |
|---|---|
| `GIT_TAG` in `FetchContent_Declare(juce_tevirtualmidi ...)` | The [juce-tevirtualmidi](https://github.com/dcroper/juce-tevirtualmidi/releases) release, such as `v1.0.0` |
| `WAVES_JUCE_VERSION` and `WAVES_JUCE_MAJOR_VERSION` | The JUCE release, such as `9.0.6`, and which patch file to use |

To update either one:
1. Check the juce-tevirtualmidi [changelog](https://github.com/dcroper/juce-tevirtualmidi/blob/main/CHANGELOG.md) for a release that supports the JUCE version you want.
1. Change the settings above.
1. Delete the `build` folder, so CMake downloads and patches the new versions, then build and test on macOS and Windows.

## Adding a setting
Saved settings live in `Source/Settings/`:

| File | Job |
|---|---|
| `AppSettings.h` / `.cpp` | The `AppSettings` struct: every saved setting, with its default, and how it's read from and written to JSON |
| `SettingsStore.h` / `.cpp` | Loads the JSON file at startup, and saves it about a second after each change and again on quit |
| `DeviceChoices.h` | Which MIDI devices are selected at startup, and what's saved when one is picked |

The file has a `schemaVersion` in the form `major.minor`. The app's own is `AppSettings::currentSchema`.

| Change | Version bump | Example |
|---|---|---|
| Add a field, or a new value for a field | Minor (`1.0` → `1.1`) | A new `scaleRoot` setting, or a new `type` value |
| Rename or remove a field, or change what a value means | Major (`1.1` → `2.0`) | Renaming `midiOutput` |

| File's `schemaVersion` compared with the app's | What the app does |
|---|---|
| Same major, any minor | Reads it. Missing fields get defaults, and unknown fields and values are logged and kept. Saving writes only the settings that changed, and keeps the file's minor version if it's newer. |
| Newer major | Moves the file to `WavesTuneRTMidiController.json.bak`, and writes a new file in its own version |
| Older major | Converts it with `AppSettings::migrateToCurrentMajor()`, one major at a time |

To add a setting (a minor change):
1. Add a member with a default value to `AppSettings`.
1. Write it in `AppSettings::toVar()`, only when it changed, like the existing ones. Read it in `AppSettings::fromVar()`, and add its key to the list of known keys there.
1. Increase `AppSettings::currentSchema` by one minor version.
1. Read it with `settings.get()` and change it with `settings.update(...)`. Saving happens automatically.
1. Add a check for it to `Tests/SettingsTests.cpp`, for example to the JSON round trip test.

For a major change, increase the major version, set the minor to `0`, and add the conversion from the previous major to `AppSettings::migrateToCurrentMajor()`, with a test.

If the file isn't valid JSON, or its `schemaVersion` isn't valid, the app renames it to `WavesTuneRTMidiController.json.bak` and starts with defaults. If saving fails, the change stays pending and is tried again on the next change or when the app quits.

## Working on the patch
To build against a local copy of juce-tevirtualmidi instead of the one on GitHub, start from an empty `build` folder and run:
```
cmake -B build -DFETCHCONTENT_SOURCE_DIR_JUCE_TEVIRTUALMIDI=../juce-tevirtualmidi
```
