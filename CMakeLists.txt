cmake_minimum_required(VERSION 3.22)

# Build a universal (Apple silicon + Intel) app on macOS unless told otherwise.
# These must be set before project().
if(NOT DEFINED CMAKE_OSX_ARCHITECTURES)
    set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "macOS architectures to build")
endif()
set(CMAKE_OSX_DEPLOYMENT_TARGET "12.0" CACHE STRING "Minimum macOS version")

project(WavesTuneRTMidiController VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#===============================================================================
# Dependencies

set(WAVES_JUCE_VERSION 9.0.6)
set(WAVES_JUCE_MAJOR_VERSION 9)

include(FetchContent)

# Patch that adds Windows virtual MIDI output support (teVirtualMIDI) to JUCE
FetchContent_Declare(juce_tevirtualmidi
    GIT_REPOSITORY https://github.com/dcroper/juce-tevirtualmidi.git
    GIT_TAG        v1.0.0
    GIT_SHALLOW    TRUE)
FetchContent_MakeAvailable(juce_tevirtualmidi)

juce_tevirtualmidi_patch_command(juce_patch_command ${WAVES_JUCE_MAJOR_VERSION})

FetchContent_Declare(JUCE
    GIT_REPOSITORY      https://github.com/juce-framework/JUCE.git
    GIT_TAG             ${WAVES_JUCE_VERSION}
    GIT_SHALLOW         TRUE
    PATCH_COMMAND       ${juce_patch_command}
    UPDATE_DISCONNECTED TRUE)
FetchContent_MakeAvailable(JUCE)

#===============================================================================
# App

juce_add_gui_app(WavesTuneRTMidiController
    PRODUCT_NAME    "WavesTuneRTMidiController"
    VERSION         ${PROJECT_VERSION}
    COMPANY_WEBSITE "https://github.com/dcroper"
    BUNDLE_ID       "ca.droper.WavesTuneRTMidiController"
    ICON_BIG        "${CMAKE_CURRENT_SOURCE_DIR}/Source/iconLarge.png")

juce_generate_juce_header(WavesTuneRTMidiController)

target_sources(WavesTuneRTMidiController PRIVATE
    Source/Main.cpp
    Source/Components/MainComponent.cpp
    Source/Components/MidiSourceSelectComponent.cpp
    Source/Components/ScaleSelector.cpp
    Source/Components/ToggleButtonGroup.cpp
    Source/Constants/ScaleConstants.cpp
    Source/Fonts/Fonts.cpp
    Source/Managers/MidiKeyboardManager.cpp
    Source/Managers/MidiOutputManager.cpp)

juce_add_binary_data(WavesTuneRTMidiControllerData SOURCES
    Source/iconLarge.png
    Source/Fonts/GothicA1/GothicA1-Black.ttf
    Source/Fonts/GothicA1/GothicA1-Bold.ttf
    Source/Fonts/GothicA1/GothicA1-ExtraBold.ttf
    Source/Fonts/GothicA1/GothicA1-ExtraLight.ttf
    Source/Fonts/GothicA1/GothicA1-Light.ttf
    Source/Fonts/GothicA1/GothicA1-Medium.ttf
    Source/Fonts/GothicA1/GothicA1-Regular.ttf
    Source/Fonts/GothicA1/GothicA1-SemiBold.ttf
    Source/Fonts/GothicA1/GothicA1-Thin.ttf)

target_compile_definitions(WavesTuneRTMidiController PRIVATE
    JUCE_STRICT_REFCOUNTEDPOINTER=1
    JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:WavesTuneRTMidiController,JUCE_PRODUCT_NAME>"
    JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:WavesTuneRTMidiController,JUCE_VERSION>")

target_link_libraries(WavesTuneRTMidiController
    PRIVATE
        WavesTuneRTMidiControllerData
        juce::juce_audio_devices
        juce::juce_gui_basics
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)
