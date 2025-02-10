#include "AudioBuffer.h"

#include <algorithm>


namespace hidonash::core
{
    const float& AudioBuffer::Channel::operator[](size_t sample) const
    {
        return buffer_[sample];
    }

    float& AudioBuffer::Channel::operator[](size_t sample)
    {
        return buffer_[sample];
    }

    size_t AudioBuffer::Channel::size() const
    {
        return size_;
    }

    void AudioBuffer::Channel::fill(float value)
    {
        for (size_t sa = 0; sa < size_; ++sa)
            buffer_[sa] = value;
    }

    void AudioBuffer::Channel::applyGain(float gain)
    {
        for (size_t sa = 0; sa < size_; ++sa)
            buffer_[sa] *= gain;
    }

    AudioBuffer::Channel::Channel(float* buffer, size_t size)
    : buffer_(buffer)
    , size_(size)
    {}

    AudioBuffer::AudioBuffer(int numChannels, int numSamples)
    : numChannels_(numChannels)
    , numSamples_(numSamples)
    , memoryBlock_(numChannels, numSamples)
    , data_(memoryBlock_.getData())
    {}

    AudioBuffer::AudioBuffer(float* const* dataToReferTo, int numChannels, int numSamples)
    : numChannels_(numChannels)
    , numSamples_(numSamples)
    , data_(dataToReferTo)
    {}

    float AudioBuffer::getSample(int channel, int sample) const
    {
        return *(data_[channel] + sample);
    }

    void AudioBuffer::setSample(int destChannel, int destSample, float newValue)
    {
        *(data_[destChannel] + destSample) = newValue;
    }

    size_t AudioBuffer::getNumChannels() const
    {
        return numChannels_;
    }

    size_t AudioBuffer::getNumSamples() const
    {
        return numSamples_;
    }

    float* AudioBuffer::getDataPointer() const
    {
        return *data_;
    }

    const std::unique_ptr<IAudioBuffer::IChannel> AudioBuffer::getChannel(size_t channel) const
    {
        if (channel >= numChannels_)
            return std::make_unique<Channel>(nullptr, 0);

        return std::make_unique<Channel>(data_[channel], numSamples_);
    }

    std::unique_ptr<IAudioBuffer::IChannel> AudioBuffer::getChannel(size_t channel)
    {
        if (channel >= numChannels_)
            return std::make_unique<Channel>(nullptr, 0);

        return std::make_unique<Channel>(data_[channel], numSamples_);
    }

    void AudioBuffer::fill(float value)
    {
        for (size_t ch = 0; ch < numChannels_; ++ch)
            for (size_t sa = 0; sa < numSamples_; ++sa)
                setSample(ch, sa, value);
    }

    void AudioBuffer::copyFrom(const IAudioBuffer& other)
    {
        if (other.getNumChannels() == numChannels_ && other.getNumSamples() == numSamples_)
            memcpy(data_[0], other.getDataPointer(), numChannels_ * numSamples_ * sizeof(float));
        else
        {
            const auto channelsToCopy =
                static_cast<size_t>(std::min(static_cast<int>(other.getNumChannels()), static_cast<int>(numChannels_)));
            const auto samplesToCopy = std::min(static_cast<int>(numSamples_), static_cast<int>(other.getNumSamples()));

            for (size_t c = 0; c < channelsToCopy; ++c)
                memcpy(data_[c], other.getDataPointer(), samplesToCopy * sizeof(float));
        }
    }

    void AudioBuffer::copy(const IAudioBuffer& from, size_t fromOffset, size_t internalOffset, size_t copyLength)
    {
        memcpy(getDataPointer() + internalOffset, from.getDataPointer() + fromOffset, copyLength * sizeof(float));
    }

    void AudioBuffer::add(const IAudioBuffer& from, const size_t addLength, const size_t fromOffset,
                          const size_t internalOffset)
    {
        const auto numChannels = std::min(numChannels_, from.getNumChannels());

        for (size_t c = 0; c < numChannels; ++c)
            for (size_t i = 0; i < addLength; ++i)
                data_[c][i + internalOffset] += from.getSample(c, i + fromOffset);
    }

    void AudioBuffer::multiply(float value, size_t multiplyLength)
    {
        for (size_t c = 0; c < getNumChannels(); ++c)
            for (size_t i = 0; i < multiplyLength; ++i)
                data_[c][i] *= value;
    }

    void AudioBuffer::multiply(const std::vector<float>& from, size_t multiplyLength)
    {
        const auto numChannels =
            static_cast<size_t>(std::min(static_cast<int>(numChannels_), static_cast<int>(from.size())));

        for (size_t c = 0; c < numChannels; ++c)
            for (size_t i = 0; i < multiplyLength; ++i)
                data_[c][i] *= from[i];
    }

    void AudioBuffer::setSize(size_t channelCount, size_t sampleCountPerChannel)
    {
        memoryBlock_ = MemoryBlock(channelCount, sampleCountPerChannel);
        data_ = memoryBlock_.getData();
        numChannels_ = channelCount;
        numSamples_ = sampleCountPerChannel;
    }
}
