cmake_minimum_required(VERSION 3.22)

# ==============================================================================
# ModelW Precision Dynamics Processor - CMake Build System
# Target Platforms: Windows x64 (VST3/Standalone) & macOS Apple Silicon (AU/VST3)
# ==============================================================================

project(ModelW VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ------------------------------------------------------------------------------
# Universal macOS Architectures (Apple Silicon M-series ARM64 + Intel x86_64)
# ------------------------------------------------------------------------------
if(APPLE)
    if(NOT CMAKE_OSX_ARCHITECTURES)
        set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "Target architectures" FORCE)
    endif()
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15" CACHE STRING "Minimum macOS version" FORCE)
endif()

# ------------------------------------------------------------------------------
# Fetch JUCE Framework via CMake FetchContent
# ------------------------------------------------------------------------------
include(FetchContent)

message(STATUS "Fetching JUCE Framework...")
FetchContent_Declare(
    juce
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        8.0.4
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(juce)

# ------------------------------------------------------------------------------
# Plugin Definition
# ------------------------------------------------------------------------------
juce_add_plugin(ModelW
    COMPANY_NAME "ModelW Audio"
    BUNDLE_ID "com.modelwaudio.modelw"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    PLUGIN_MANUFACTURER_CODE ModW
    PLUGIN_CODE MdlW
    FORMATS VST3 AU Standalone
    PRODUCT_NAME "ModelW"
    DESCRIPTION "ModelW Precision Dynamics & De-Esser Processor"
)

# ------------------------------------------------------------------------------
# Target Sources & Includes
# ------------------------------------------------------------------------------
target_sources(ModelW
    PRIVATE
        Source/ModelW_DSP.h
        Source/PluginProcessor.h
        Source/PluginProcessor.cpp
        Source/PluginEditor.h
        Source/PluginEditor.cpp
)

target_include_directories(ModelW
    PRIVATE
        Source
)

# ------------------------------------------------------------------------------
# Link Required JUCE Modules
# ------------------------------------------------------------------------------
target_link_libraries(ModelW
    PRIVATE
        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_gui_basics
        juce::juce_gui_extra
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

# ------------------------------------------------------------------------------
# Compile Definitions & Optimizations
# ------------------------------------------------------------------------------
target_compile_definitions(ModelW
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

if(MSVC)
    target_compile_options(ModelW PRIVATE /W4 /MP /fp:fast)
else()
    target_compile_options(ModelW PRIVATE -Wall -Wextra -ffast-math)
endif()
