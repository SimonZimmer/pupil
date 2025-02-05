#pragma once


#include <exception>
#include <juce_gui_basics/juce_gui_basics.h>

#include <GraphicAssets.h>
#include <mutex>

#include "Font.h"
#include "UiConstants.h"


namespace hidonash
{
    class TextBox : public juce::Component,
                    public juce::Slider::Listener
    {
    public:
        explicit TextBox(juce::Slider& slider)
        : slider_(slider)
        {
            fontOptions_ = Font::chicagoFLF();
            label_.setJustificationType(juce::Justification::right);
            label_.setEditable(false, false, false);
            label_.setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
            label_.setColour(juce::Label::outlineColourId, juce::Colours::transparentBlack);
            label_.setColour(juce::TextEditor::backgroundColourId, juce::Colours::transparentBlack);
            label_.setColour(juce::TextEditor::textColourId, uiconstants::baseColour);
            label_.setColour(juce::Label::ColourIds::textColourId, uiconstants::baseColour);

            updateFromSlider();

            label_.onEditorShow = [this] {
                label_.getCurrentTextEditor()->setInputRestrictions(0, "-.0123456789");
            };

            label_.onTextChange = [this] {
                const auto value = slider_.getRange().clipValue(label_.getText(false).getDoubleValue());

                if (slider_.getValue() != value)
                    slider_.setValue(value, juce::sendNotificationSync);
                else
                    updateFromSlider();
            };

            slider_.addListener(this);

            addAndMakeVisible(label_);
        }

        ~TextBox()
        {
            setLookAndFeel(nullptr);
        }

        void resized() override
        {
            label_.setBounds(getLocalBounds());
            label_.setFont(fontOptions_.withHeight(slider_.getWidth() / 6.f));
        }

    private:
        void sliderValueChanged(juce::Slider* slider) override
        {
            if (slider != &slider_)
                return;

            updateFromSlider();
        }

        void updateFromSlider()
        {
            label_.setText(juce::String(slider_.getValue(), 2), juce::dontSendNotification);
        }

        juce::Slider& slider_;
        juce::Label label_;
        juce::FontOptions fontOptions_;
    };
}
