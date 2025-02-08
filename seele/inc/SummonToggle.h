#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include <GraphicAssets.h>

#include "UiConstants.h"


namespace hidonash
{
    class ToggleButtonLookAndFeel : public juce::LookAndFeel_V4
    {
    public:
        ToggleButtonLookAndFeel()
        {
            setColour(juce::ToggleButton::ColourIds::textColourId, juce::Colours::transparentBlack);
        }

        void drawToggleButton(juce::Graphics& graphics, juce::ToggleButton& button, bool, bool) override
        {
            const auto enabledColour = juce::Colour::fromRGBA(255, 245, 255, 255);
            const auto disabledColour = juce::Colour::fromRGBA(255, 245, 255, 30);

            if (button.getToggleState())
                graphics.setColour(enabledColour);
            else
                graphics.setColour(disabledColour);

            const auto buttonBounds = button.getLocalBounds().toFloat();
            graphics.fillRect(buttonBounds);

            graphics.setFont(uiconstants::font);
            graphics.setFont(10.f);
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
    };
}
