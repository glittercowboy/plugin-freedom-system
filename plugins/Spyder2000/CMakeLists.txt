cmake_minimum_required(VERSION 3.15)

# Plugin formats: VST3, AU, Standalone
juce_add_plugin(Spyder2000
    COMPANY_NAME "YourCompany"
    PLUGIN_MANUFACTURER_CODE Manu
    PLUGIN_CODE Spy2
    FORMATS VST3 AU Standalone
    PRODUCT_NAME "Spyder 2000"
)

# Source files
target_sources(Spyder2000
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
)

# Include paths
target_include_directories(Spyder2000
    PRIVATE
        Source
)

# Required JUCE modules
target_link_libraries(Spyder2000
    PRIVATE
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_audio_plugin_client
        juce::juce_audio_processors
        juce::juce_audio_utils
        juce::juce_core
        juce::juce_data_structures
        juce::juce_dsp
        juce::juce_events
        juce::juce_graphics
        juce::juce_gui_basics
        juce::juce_gui_extra
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

# Generate JuceHeader.h (JUCE 8 requirement - CRITICAL: After target_link_libraries)
juce_generate_juce_header(Spyder2000)

# Compile definitions
target_compile_definitions(Spyder2000
    PUBLIC
        JUCE_VST3_CAN_REPLACE_VST2=0
)
