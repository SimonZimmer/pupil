cmake_minimum_required(VERSION 3.13)
project(seelePlugin)

if (NOT TARGET juceaide)
    add_subdirectory(../ThirdParty/JUCE juce)
endif()

add_subdirectory(seeleCore)
add_subdirectory(resources)

set(PLUGIN_NAME "SEELE")
set(FORMATS VST3)
juce_add_plugin(seelePlugin
        PLUGIN_MANUFACTURER_CODE HDNS
        PLUGIN_CODE SLPL
        FORMATS ${FORMATS}
        PRODUCT_NAME ${PLUGIN_NAME}
        COMPANY_NAME "hidonash"
        VERSION 1.0
        )

target_compile_features(seelePlugin PUBLIC cxx_std_17)

target_sources(seelePlugin PRIVATE
        src/PluginEditor.cpp
        src/PluginProcessor.cpp
        src/MainComponent.cpp
        src/MemberArea.cpp
        src/Oscilloscope.cpp
        )

target_include_directories(seelePlugin PRIVATE inc)

target_compile_definitions(seelePlugin PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        )

target_link_libraries(seelePlugin
        PRIVATE
            juce::juce_core
            juce::juce_audio_utils
            seeleCore
            core
            GraphicAssets
        )

set(common_definitions
        JUCE_QUICKTIME=0
        JUCE_PLUGINHOST_AU=0
        JUCE_PLUGINHOST_AAX=0
        )
