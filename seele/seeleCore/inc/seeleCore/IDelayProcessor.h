#pragma once

#include <memory>

#include "IChannelProcessor.h"


namespace hidonash
{
    class IDelayProcessor : public IChannelProcessor
    {
    public:
        virtual ~IDelayProcessor() = default;

        virtual void setDelayInSeconds(float delayInSeconds) = 0;
    };

    using DelayProcessorPtr = std::unique_ptr<IDelayProcessor>;
}
