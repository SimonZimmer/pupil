#include "MemberArea.h"

#include <seeleCore/Config.h>

#include "MemberArea.h"
#include "SummonToggle.h"
#include "TextBox.h"


namespace hidonash
{
    void MemberArea::resized()
    {
        auto localBounds = getLocalBounds();
        const auto singleMemberWidth = getWidth() / config::constants::numMembers;
        const auto subsectionHeight = localBounds.getHeight() / 20.f;
        const auto numSubSections = 3;

        for (auto n = 0; n < config::constants::numMembers; ++n)
        {
            sanctitySliders_.emplace_back(std::make_unique<SeeleSlider>(n + 1));
            auto singleMemberBounds = localBounds.removeFromLeft(singleMemberWidth);
            auto sliderBounds = singleMemberBounds.removeFromLeft(singleMemberWidth);
            const int widthPadding = singleMemberWidth / (2 * numSubSections);
            sliderBounds.reduce(widthPadding, 0);
            sliderBounds.setHeight(localBounds.getHeight() - (2 * numSubSections) * subsectionHeight);
            sanctitySliders_[n]->setBounds(sliderBounds);
            addAndMakeVisible(*sanctitySliders_[n]);

            auto subSectionBounds = sliderBounds;
            subSectionBounds.removeFromBottom(sliderBounds.getHeight() - subsectionHeight);

            distanceSliders_.emplace_back(
                std::make_unique<MiniSlider>("DISTANCE", "Distance (delay in s) of the SEELE Member"));
            distanceSliders_[n]->setBounds(subSectionBounds);
            distanceSliders_[n]->setTopLeftPosition(sliderBounds.getX(),
                                                    sliderBounds.getY() + sliderBounds.getHeight() + subsectionHeight);
            addAndMakeVisible(*distanceSliders_[n]);

            distanceTextBoxes_.emplace_back(std::make_unique<TextBox>(*distanceSliders_[n]));
            distanceTextBoxes_[n]->setBounds(subSectionBounds);
            distanceTextBoxes_[n]->setTopLeftPosition(sliderBounds.getX() + 0.05f * sliderBounds.getWidth(),
                                                      sliderBounds.getY() + sliderBounds.getHeight() +
                                                          1.4f * subsectionHeight);
            addAndMakeVisible(*distanceTextBoxes_[n]);

            gainSliders_.emplace_back(std::make_unique<MiniSlider>("LEVEL", "Level (gain in dB) of the SEELE Member"));
            gainSliders_[n]->setBounds(subSectionBounds);
            gainSliders_[n]->setTopLeftPosition(sliderBounds.getX(),
                                                sliderBounds.getY() + sliderBounds.getHeight() + 3 * subsectionHeight);
            addAndMakeVisible(*gainSliders_[n]);

            gainTextBoxes_.emplace_back(std::make_unique<TextBox>(*gainSliders_[n]));
            gainTextBoxes_[n]->setBounds(subSectionBounds);
            gainTextBoxes_[n]->setTopLeftPosition(sliderBounds.getX() + 0.05f * sliderBounds.getWidth(),
                                                  sliderBounds.getY() + sliderBounds.getHeight() +
                                                      3.4f * subsectionHeight);
            addAndMakeVisible(*gainTextBoxes_[n]);

            summonToggles_.emplace_back(std::make_unique<SummonToggle>());
            summonToggles_[n]->setBounds(subSectionBounds);
            summonToggles_[n]->setTopLeftPosition(sliderBounds.getX(), sliderBounds.getY() + sliderBounds.getHeight() +
                                                                           5 * subsectionHeight);
            addAndMakeVisible(*summonToggles_[n]);
        }
    }

    SeeleSlider& MemberArea::getSanctitySlider(size_t index)
    {
        return *sanctitySliders_[index];
    }

    SummonToggle& MemberArea::getSummonToggle(size_t index)
    {
        return *summonToggles_[index];
    }

    MiniSlider& MemberArea::getDistanceSlider(size_t index)
    {
        return *distanceSliders_[index];
    }

    MiniSlider& MemberArea::getGainSlider(size_t index)
    {
        return *gainSliders_[index];
    }
}
