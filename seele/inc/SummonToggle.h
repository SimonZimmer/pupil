#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "Font.h"
#include <GraphicAssets.h>


namespace hidonash
{
    class ToggleButtonLookAndFeel : public juce::LookAndFeel_V4
    {
    public:
        ToggleButtonLookAndFeel()
        {
            setColour(juce::ToggleButton::ColourIds::textColourId, juce::Colours::transparentBlack);
        }

        void drawToggleButton(juce::Graphics& g, juce::ToggleButton& button, bool, bool) override
        {
            const auto enabledColour = juce::Colour::fromRGBA(255, 245, 255, 255);
            const auto disabledColour = juce::Colour::fromRGBA(255, 245, 255, 30);

            if (button.getToggleState())
                g.setColour(enabledColour);
            else
                g.setColour(disabledColour);

            const auto buttonBounds = button.getLocalBounds().toFloat();
            g.fillRect(buttonBounds);

            g.setFont(Font::chicagoFLF());
            g.setFont(10.f);
        }
    };

    class SummonToggle : public juce::ToggleButton
    {
    public:
        SummonToggle()
        {
            setLookAndFeel(&lookAndFeel_);
        }

        ~SummonToggle()
        {
            setLookAndFeel(nullptr);
        }

        void mouseEnter(const juce::MouseEvent&) override
        {
            setMouseCursor(juce::MouseCursor::StandardCursorType::PointingHandCursor);
        }

        void mouseExit(const juce::MouseEvent&) override
        {
            setMouseCursor(juce::MouseCursor::StandardCursorType::NormalCursor);
        }

    private:
        ToggleButtonLookAndFeel lookAndFeel_ {};
        juce::GlowEffect glowEffect_ {};
    };
}
