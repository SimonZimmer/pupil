#pragma once

#include <memory>
#include <vector>


namespace hidonash::core
{
    class IAudioBuffer
    {
    public:
        class IChannel
        {
        public:
            virtual const float& operator[](size_t sample) const = 0;

            virtual ~IChannel() = default;

            virtual float& operator[](size_t sample) = 0;

            virtual size_t size() const = 0;

            virtual void fill(float value) = 0;

            virtual void applyGain(float gain) = 0;
        };

        virtual ~IAudioBuffer() = default;

        virtual float getSample(int channel, int sample) const = 0;

        virtual void setSample(int destChannel, int destSample, float newValue) = 0;

        virtual size_t getNumChannels() const = 0;

        virtual size_t getNumSamples() const = 0;

        virtual float* getDataPointer() const = 0;

        virtual const std::unique_ptr<IAudioBuffer::IChannel> getChannel(size_t channel) const = 0;

        virtual std::unique_ptr<IAudioBuffer::IChannel> getChannel(size_t channel) = 0;

        virtual void fill(float value) = 0;

        virtual void copyFrom(const IAudioBuffer& other) = 0;

        virtual void copy(const IAudioBuffer& from, size_t fromOffset, size_t internalOffset, size_t copyLength) = 0;

        virtual void add(const IAudioBuffer& from, const size_t addLength, const size_t fromOffset = 0,
                         const size_t internalOffset = 0) = 0;

        virtual void multiply(float value, size_t multiplyLength) = 0;

        virtual void multiply(const std::vector<float>& from, size_t multiplyLength) = 0;

        virtual void setSize(size_t channelCount, size_t sampleCountPerChannel) = 0;
    };

    using AudioBufferPtr = std::unique_ptr<IAudioBuffer>;
}
