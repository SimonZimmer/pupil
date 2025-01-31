#pragma once

#include <gmock/gmock.h>

#include <seeleCore/IDelayProcessor.h>


namespace hidonash
{
    class DelayProcessorMock : public IDelayProcessor
    {
    public:
        ~DelayProcessorMock() = default;

        MOCK_METHOD(void, setDelayInSeconds, (int) );

        MOCK_METHOD(void, process, (core::IAudioBuffer::IChannel&) );
    };
}
