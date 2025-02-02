#include <juce_graphics/juce_graphics.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include <GraphicAssets.h>
#include <seeleCore/Config.h>

#include "Font.h"
#include "UiConstants.h"


namespace hidonash
{
    namespace
    {
        class SliderLookAndFeel : public juce::LookAndFeel_V4
        {
        public:
            SliderLookAndFeel(int memberNumber)
            : memberIdentifier_("0" + std::to_string(memberNumber))
            {
                setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colours::transparentBlack);
            }

            void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                                  float minSliderPos, float maxSliderPos, const juce::Slider::SliderStyle style,
                                  juce::Slider& slider) override
            {
                g.fillAll(juce::Colour::fromRGB(40, 40, 70));
                const auto sliderWidth = slider.getWidth();

                auto path = juce::Path();
                path.addRectangle(static_cast<float>(x), sliderPos, static_cast<float>(height),
                                  1.f + static_cast<float>(height) - sliderPos);

                auto baseColour = juce::Colour::fromRGB(110, 104, 162)
                                      .withMultipliedSaturation(slider.isEnabled() ? 1.f : 0.5f)
                                      .withMultipliedAlpha(0.8f);

                g.setGradientFill(juce::ColourGradient::vertical(baseColour.brighter(0.2f), 0.f, baseColour.darker(1.f),
                                                                 static_cast<float>(height)));
                g.fillPath(path);
                g.setColour(baseColour.darker(0.2f));
                g.fillRect(static_cast<float>(x), sliderPos, static_cast<float>(sliderWidth), 1.f);

                g.setColour(currentColour_);
                g.setFont(Font::chicagoFLF());
                g.setFont(sliderWidth / 5.f);
                g.drawMultiLineText("SEELE\n\n\n\nSOUND\nONLY", sliderWidth * 0.1f, height * 0.1, sliderWidth * 0.9,
                                    juce::Justification::centred);
                g.drawText("SANCTITY",
                           juce::Rectangle<int>(sliderWidth * 0.05f, height * 0.8, sliderWidth * 0.9, height * 0.25),
                           juce::Justification::centred);
                g.setFont(sliderWidth / 2.5f);
                g.drawText(memberIdentifier_,
                           juce::Rectangle<int>(sliderWidth * 0.1f, height * 0.055, sliderWidth * 0.9, height * 0.2),
                           juce::Justification::centred);
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
            const std::string memberIdentifier_;
            const juce::Colour baseColour_ {uiconstants::baseColour};
            const juce::Colour highlightColour_ {uiconstants::highlightColour};
            juce::Colour currentColour_ {baseColour_};
        };
    }

    class SeeleSlider : public juce::Slider
    {
    public:
        SeeleSlider(int memberNumber)
        : lookAndFeel_(memberNumber)
        {
            setSliderStyle(juce::Slider::LinearBarVertical);
            setTextBoxStyle(NoTextBox, false, 0, 0);
            setDoubleClickReturnValue(true, config::parameters::defaultPitchFactor);
            setRange(0.f, 1.f, 0.01f);
            setTooltip("Sanctity of the Seele Member");
            setLookAndFeel(&lookAndFeel_);
        }

        ~SeeleSlider() override
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
        SliderLookAndFeel lookAndFeel_;
    };
}
