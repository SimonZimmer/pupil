#include "Oscilloscope.h"


namespace hidonash
{
    Oscilloscope::Oscilloscope(NewProjectAudioProcessor& processor, size_t numSamples, int refreshRateHz)
    : processor_(processor)
    , displayBuffer_(1, numSamples)
    {
        startTimer(refreshRateHz);
    }

    void Oscilloscope::paint(juce::Graphics& graphics)
    {
        graphics.setOpacity(1.0f);
        graphics.setColour(juce::Colours::red);

        const auto size = displayBuffer_.getNumSamples();
        auto&& readPointer = displayBuffer_.getReadPointer(0);

        for (int i = 1; i < size; ++i)
        {
            const auto width = getLocalBounds().getWidth();
            const auto height = getLocalBounds().getHeight();

            graphics.drawLine({(float) juce::jmap(i - 1, 0, size - 1, 0, width),
                               juce::jmap(readPointer[i - 1], -1.0f, 1.0f, (float) height, 0.0f),
                               (float) juce::jmap(i, 0, size - 1, 0, width),
                               juce::jmap(readPointer[i], -1.0f, 1.0f, (float) height, 0.0f)},
                              2);
        }
    }

    void Oscilloscope::timerCallback()
    {
        processor_.getLatestAudioBlock(displayBuffer_.getWritePointer(0), displayBuffer_.getNumSamples());
        repaint();
    }
}
