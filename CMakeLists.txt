cmake_minimum_required(VERSION 3.22)

project(AutumnAftertouchEnsemble VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

FetchContent_Declare(
    juce
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.10
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
)

FetchContent_MakeAvailable(juce)

juce_add_plugin(AutumnAftertouchEnsemble
    COMPANY_NAME "MPJRecords"
    PLUGIN_MANUFACTURER_CODE MPJR
    PLUGIN_CODE A23s
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    PLUGIN_NAME "Autumn Aftertouch Ensemble"
    PRODUCT_NAME "AutumnAftertouchEnsemble"
    FORMATS VST3 Standalone
    VST3_CATEGORIES Instrument Synth
)

target_sources(AutumnAftertouchEnsemble
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
)

target_compile_definitions(AutumnAftertouchEnsemble
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(AutumnAftertouchEnsemble
    PRIVATE
        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
