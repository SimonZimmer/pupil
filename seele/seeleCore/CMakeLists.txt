cmake_minimum_required(VERSION 3.13)

project(seeleCore)

add_library(seeleCore
    STATIC
        src/Factory.cpp
        src/Engine.cpp
        src/PitchShifter.cpp
        src/PitchShifterManager.cpp
        src/DelayProcessor.cpp
        src/GainProcessor.cpp
        src/Analysis.cpp
        src/Synthesis.cpp
        src/MemberParameterSet.cpp
        )

target_include_directories(seeleCore
    PRIVATE
        inc/seeleCore
    PUBLIC
        inc
    )

target_link_libraries(seeleCore
   PUBLIC
        core
        juce::juce_dsp
        )

target_compile_features(seeleCore PUBLIC cxx_std_17)

enable_testing()
add_subdirectory(tests)
add_subdirectory(mocks)
