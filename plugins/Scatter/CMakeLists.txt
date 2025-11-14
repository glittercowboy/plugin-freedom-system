cmake_minimum_required(VERSION 3.15)

# Plugin formats: VST3, AU, Standalone
juce_add_plugin(Scatter
    COMPANY_NAME "YourCompany"
    PLUGIN_MANUFACTURER_CODE Manu
    PLUGIN_CODE Scat
    FORMATS VST3 AU Standalone
    PRODUCT_NAME "Scatter"
)

# Source files
target_sources(Scatter
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
)

# Include paths
target_include_directories(Scatter
    PRIVATE
        Source
)

# Required JUCE modules
target_link_libraries(Scatter
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

# Generate JuceHeader.h (JUCE 8 requirement - Pattern #1)
juce_generate_juce_header(Scatter)

# WebView UI Resources (uncommented by gui-agent if WebView UI used)
# juce_add_binary_data(Scatter_UIResources
#     SOURCES
#         Source/ui/public/index.html
#         Source/ui/public/js/juce/index.js
# )
#
# target_link_libraries(Scatter
#     PRIVATE
#         Scatter_UIResources
# )

# Compile definitions
target_compile_definitions(Scatter
    PUBLIC
        JUCE_VST3_CAN_REPLACE_VST2=0
)

# WebView support (uncommented by gui-agent if WebView UI used)
# target_compile_definitions(Scatter
#     PUBLIC
#         JUCE_WEB_BROWSER=1
#         JUCE_USE_CURL=0
# )
