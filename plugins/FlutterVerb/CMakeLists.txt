cmake_minimum_required(VERSION 3.15)

# Plugin formats: VST3, AU, Standalone
juce_add_plugin(FlutterVerb
    COMPANY_NAME "LexDev"
    PLUGIN_MANUFACTURER_CODE Lexd
    PLUGIN_CODE Fltv
    FORMATS VST3 AU Standalone
    PRODUCT_NAME "FlutterVerb"
    NEEDS_WEB_BROWSER TRUE
)

# Source files (minimal for Stage 2)
target_sources(FlutterVerb
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
)

# Include paths
target_include_directories(FlutterVerb
    PRIVATE
        Source
)

# Required JUCE modules
target_link_libraries(FlutterVerb
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

# CRITICAL: Generate JuceHeader.h (JUCE 8 requirement)
juce_generate_juce_header(FlutterVerb)

# Compile definitions
target_compile_definitions(FlutterVerb
    PUBLIC
        JUCE_VST3_CAN_REPLACE_VST2=0
)

# WebView UI Resources (commented out until Stage 5)
# juce_add_binary_data(FlutterVerb_UIResources
#     SOURCES
#         Source/ui/public/index.html
#         Source/ui/public/js/juce/index.js
#         Source/ui/public/js/juce/check_native_interop.js
# )
#
# target_link_libraries(FlutterVerb
#     PRIVATE
#         FlutterVerb_UIResources
# )

# WebView support (commented out until Stage 5)
# target_compile_definitions(FlutterVerb
#     PUBLIC
#         JUCE_WEB_BROWSER=1
#         JUCE_USE_CURL=0
# )
