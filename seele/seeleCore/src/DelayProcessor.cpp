#include "DelayProcessor.h"


namespace hidonash
{
    namespace
    {
        constexpr auto interpolationRate = 0.001f;
    }

    DelayProcessor::DelayProcessor(float maxDelaySeconds, float delaySeconds, double sampleRate)
    : sampleRate_(sampleRate)
    , circularBuffer_(convertSecondsToSamples(maxDelaySeconds) + 1, 0.0f)
    , writeIndex_(0)
    , readPosition_(0.0f)
    , currentDelay_(convertSecondsToSamples(delaySeconds))
    , targetDelay_(convertSecondsToSamples(delaySeconds))
    {
        readPosition_ =
            fmod(writeIndex_ - currentDelay_ + circularBuffer_.size(), static_cast<float>(circularBuffer_.size()));
    }

    void DelayProcessor::process(core::IAudioBuffer::IChannel& input)
    {
        const auto bufferSize = input.size();
        for (auto sa = 0; sa < bufferSize; ++sa)
        {
            circularBuffer_[writeIndex_] = input[sa];

            if (currentDelay_ != targetDelay_)
            {
                const float delta = targetDelay_ - currentDelay_;
                currentDelay_ += delta * interpolationRate;

                if (std::abs(delta) < 0.01f)
                    currentDelay_ = targetDelay_;
            }

            readPosition_ = writeIndex_ - currentDelay_;

            while (readPosition_ < 0)
                readPosition_ += circularBuffer_.size();
            while (readPosition_ >= circularBuffer_.size())
                readPosition_ -= circularBuffer_.size();

            const int readIndex = static_cast<int>(readPosition_);
            const float fraction = readPosition_ - readIndex;

            const float sample1 = circularBuffer_[readIndex];
            const float sample2 = circularBuffer_[(readIndex + 1) % circularBuffer_.size()];
            input[sa] = sample1 + fraction * (sample2 - sample1);

            writeIndex_++;
            writeIndex_ %= circularBuffer_.size();
        }
    }

    void DelayProcessor::setDelayInSeconds(int delayInSeconds)
    {
        const auto delayInSamples = convertSecondsToSamples(delayInSeconds);

        if (delayInSamples >= circularBuffer_.size() - 1)
            return;

        targetDelay_ = delayInSamples;
    }

    float DelayProcessor::convertSecondsToSamples(float seconds)
    {
        return static_cast<float>(sampleRate_ * static_cast<double>(seconds));
    }
}
