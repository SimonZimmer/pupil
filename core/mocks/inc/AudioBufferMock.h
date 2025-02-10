#pragma once

#include <gmock/gmock.h>


#include <core/IAudioBuffer.h>


namespace hidonash::core
{
    class AudioBufferMock : public IAudioBuffer
    {
    public:
        ~AudioBufferMock() = default;

        MOCK_METHOD(float, getSample, (int, int), (const));

        MOCK_METHOD(void, setSample, (int, int, float) );

        MOCK_METHOD(size_t, getNumChannels, (), (const));

        MOCK_METHOD(size_t, getNumSamples, (), (const));

        MOCK_METHOD(float*, getDataPointer, (), (const));

        MOCK_METHOD(const std::unique_ptr<IAudioBuffer::IChannel>, getChannel, (size_t channel), (const));

        MOCK_METHOD(std::unique_ptr<IAudioBuffer::IChannel>, getChannel, (size_t channel));

        MOCK_METHOD(void, fill, (float) );

        MOCK_METHOD(void, copyFrom, (const IAudioBuffer&) );

        MOCK_METHOD(void, copy, (const IAudioBuffer&, size_t, size_t, size_t));

        MOCK_METHOD(void, add, (const IAudioBuffer&, const size_t, const size_t, const size_t));

        MOCK_METHOD(void, multiply, (float, size_t));

        MOCK_METHOD(void, multiply, (const std::vector<float>&, size_t));

        MOCK_METHOD(void, setSize, (size_t, size_t));
    };
}
