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

            void drawLinearSlider(juce::Graphics& graphics, int x, int y, int width, int height, float sliderPos,
                                  float minSliderPos, float maxSliderPos, const juce::Slider::SliderStyle style,
                                  juce::Slider& slider) override
            {
                graphics.fillAll(juce::Colour::fromRGB(40, 40, 70));
                const auto sliderWidth = slider.getWidth();

                auto path = juce::Path();
                path.addRectangle(static_cast<float>(x), sliderPos, static_cast<float>(height),
                                  1.f + static_cast<float>(height) - sliderPos);

                const auto baseColour = juce::Colour::fromRGB(110, 104, 162)
                                            .withMultipliedSaturation(slider.isEnabled() ? 1.f : 0.5f)
                                            .withMultipliedAlpha(0.8f);

                graphics.setGradientFill(juce::ColourGradient::vertical(
                    baseColour.brighter(0.2f), 0.f, baseColour.darker(1.f), static_cast<float>(height)));
                graphics.fillPath(path);
                graphics.setColour(baseColour.darker(0.2f));
                graphics.fillRect(static_cast<float>(x), sliderPos, static_cast<float>(sliderWidth), 1.f);

                graphics.setColour(currentColour_);
                graphics.setFont(Font::chicagoFLF());
                graphics.setFont(sliderWidth / 5.f);
                graphics.drawMultiLineText("SEELE\n\n\n\nSOUND\nONLY", sliderWidth * 0.1f, height * 0.1,
                                           sliderWidth * 0.9, juce::Justification::centred);
                graphics.drawText(
                    "SANCTITY",
                    juce::Rectangle<int>(sliderWidth * 0.05f, height * 0.8, sliderWidth * 0.9, height * 0.25),
                    juce::Justification::centred);
                graphics.setFont(sliderWidth / 2.5f);
                graphics.drawText(
                    memberIdentifier_,
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
