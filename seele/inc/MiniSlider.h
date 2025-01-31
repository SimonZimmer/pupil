#include "juce_graphics/juce_graphics.h"
#include <juce_gui_basics/juce_gui_basics.h>

#include <GraphicAssets.h>
#include <seeleCore/Config.h>

#include "Font.h"
#include "UiConstants.h"


namespace hidonash
{
    namespace
    {
        class MiniSliderLookAndFeel : public juce::LookAndFeel_V4
        {
        public:
            MiniSliderLookAndFeel(const std::string& name)
            : name_(name)
            {
                setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colours::transparentBlack);
            }

            void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                                  float minSliderPos, float maxSliderPos, const juce::Slider::SliderStyle style,
                                  juce::Slider& slider) override
            {
                const auto sliderWidth = slider.getWidth();
                g.setColour(currentColour_);

                auto bigBarPath = juce::Path();
                bigBarPath.addRectangle(x, y, 1.0f + sliderPos, 0.4f * height);

                auto smallBarPath = juce::Path();
                smallBarPath.addRectangle(x, y + 0.5f * height, 1.0f + width, height * 0.1f);

                auto textBounds = juce::Rectangle<int>(x, y + 0.7f * height, 1.0f + width, height * 0.3f);

                g.fillPath(bigBarPath);
                g.fillPath(smallBarPath);
                g.setFont(Font::chicagoFLF());
                g.setFont(sliderWidth * 0.125f);
                g.drawText(name_, textBounds, juce::Justification::left);
            }

            void setHighlightColour()
            {
                currentColour_ = highlightColour_;
            }

            void setBaseColour()
            {
                currentColour_ = baseColour_;
            }

        private:
            std::string name_;
            juce::Colour baseColour_ {uiconstants::baseColour};
            juce::Colour highlightColour_ {uiconstants::highlightColour};
            juce::Colour currentColour_ {baseColour_};
        };
    }

    class MiniSlider : public juce::Slider
    {
    public:
        MiniSlider(const std::string& name, const std::string& tooltip)
        : lookAndFeel_(name)
        {
            setSliderStyle(juce::Slider::LinearBar);
            setTextBoxStyle(NoTextBox, false, 0, 0);
            setDoubleClickReturnValue(true, config::parameters::minDistanceInSeconds);
            setRange(0.f, 100.f, 0.01f);
            setTooltip(tooltip);
            setLookAndFeel(&lookAndFeel_);
        }

        ~MiniSlider() override
        {
            setLookAndFeel(nullptr);
        }

        void mouseEnter(const juce::MouseEvent& e) override
        {
            setMouseCursor(juce::MouseCursor::StandardCursorType::PointingHandCursor);
            lookAndFeel_.setHighlightColour();
        }

        void mouseExit(const juce::MouseEvent&) override
        {
            setMouseCursor(juce::MouseCursor::StandardCursorType::NormalCursor);
            lookAndFeel_.setBaseColour();
        }

    private:
        MiniSliderLookAndFeel lookAndFeel_;
    };
}
