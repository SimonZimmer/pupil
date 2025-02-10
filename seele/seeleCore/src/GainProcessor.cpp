#include "GainProcessor.h"


namespace hidonash
{
    namespace
    {
        float convertDbToGain(const float gainValue)
        {
            return std::pow(10.0f, gainValue / 20.0f);
        }
    }

    GainProcessor::GainProcessor(float gainValueDb, double sampleRate)
    : gainValue_(convertDbToGain(gainValueDb), sampleRate)
    {}

    void GainProcessor::process(core::IAudioBuffer::IChannel& input)
    {
        for (size_t sa = 0; sa < input.size(); ++sa)
        {
            input[sa] *= gainValue_;
            gainValue_++;
        }
    }

    void GainProcessor::setGainDb(float gainDb)
    {
        gainValue_ = convertDbToGain(gainDb);
    }
}
