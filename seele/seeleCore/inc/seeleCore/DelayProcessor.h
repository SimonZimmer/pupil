#pragma once

#include <core/AudioBuffer.h>
#include <core/InterpolatingValue.h>

#include "IDelayProcessor.h"


namespace hidonash
{
    class DelayProcessor : public hidonash::IDelayProcessor
    {
    public:
        DelayProcessor(float maxDelaySeconds, float delaySeconds, double sampleRate);

        void process(core::IAudioBuffer::IChannel& input) override;

        void setDelayInSeconds(int delayInSeconds) override;

    private:
        float convertSecondsToSamples(float seconds);

        double sampleRate_;
        std::vector<float> circularBuffer_;
        size_t writeIndex_;
        float readPosition_;
        float currentDelay_;
        float targetDelay_;
    };
}
