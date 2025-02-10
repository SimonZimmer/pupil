#include "MainComponent.h"

#include "juce_gui_basics/juce_gui_basics.h"

#include "MemberArea.h"

#include <GraphicAssets.h>
#include <seeleCore/Config.h>


namespace hidonash
{
    MainComponent::MainComponent(NewProjectAudioProcessor& processor)
    : seeleLogo_(juce::Drawable::createFromImageData(hidonash::resources::graphicassets::seele_frame_png,
                                                     hidonash::resources::graphicassets::seele_frame_pngSize))
    , processor_(processor)
    {
        memberArea_ = std::make_unique<hidonash::MemberArea>();
        addAndMakeVisible(memberArea_.get());

        for (size_t n = 0; n < oscilloscope_.size(); ++n)
        {
            oscilloscope_[n] = std::make_unique<hidonash::Oscilloscope>(processor_, processor_.getBlockSize());
            addAndMakeVisible(oscilloscope_[n].get());
        }
    }

    void MainComponent::resized()
    {
        const auto widthPadding = getHeight() / 30.0f;
        const auto heightPadding = getHeight() / 9.f;

        auto memberBounds = getBounds().reduced(widthPadding, heightPadding);
        memberArea_->setBounds(memberBounds);
        memberArea_->setTopLeftPosition(widthPadding, heightPadding + 0.25f * heightPadding);

        auto oscilloscopeBoundsLeft = getBounds();
        oscilloscopeBoundsLeft.removeFromLeft(getWidth() * 0.784f);
        oscilloscopeBoundsLeft.removeFromBottom(getHeight() * 0.928f);
        oscilloscopeBoundsLeft.removeFromRight(getWidth() * 0.125f);
        oscilloscopeBoundsLeft.removeFromTop(getHeight() * 0.0372f);
        oscilloscope_[0]->setBounds(oscilloscopeBoundsLeft);
        auto oscilloscopeBoundsRight = oscilloscopeBoundsLeft;
        oscilloscope_[1]->setBounds(oscilloscopeBoundsRight);
        oscilloscope_[1]->setTopLeftPosition(oscilloscopeBoundsLeft.getX() + getWidth() / 9.8f,
                                             oscilloscopeBoundsLeft.getY());
    }

    SeeleSlider& MainComponent::getSanctitySlider(size_t index)
    {
        return memberArea_->getSanctitySlider(index);
    }

    SummonToggle& MainComponent::getSummonToggle(size_t index)
    {
        return memberArea_->getSummonToggle(index);
    }

    MiniSlider& MainComponent::getDistanceSlider(size_t index)
    {
        return memberArea_->getDistanceSlider(index);
    }

    MiniSlider& MainComponent::getGainSlider(size_t index)
    {
        return memberArea_->getGainSlider(index);
    }

    void MainComponent::paint(juce::Graphics& g)
    {
        g.fillAll(juce::Colour::greyLevel(0.f));
        g.setColour(juce::Colours::red);

        seeleLogo_->drawWithin(g, getBounds().toFloat(), juce::RectanglePlacement::centred, 1.f);
    }
}
