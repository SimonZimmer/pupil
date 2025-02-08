#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "Font.h"


namespace hidonash
{
    namespace uiconstants
    {
        const juce::Colour highlightColour {juce::Colours::white};
        const juce::Colour baseColour {juce::Colours::red};
        const juce::FontOptions font {Font::chicagoFLF()};
    }
}
