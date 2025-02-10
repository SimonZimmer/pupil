#include "PitchShifterManager.h"


namespace hidonash
{
    PitchShifterManager::PitchShifterManager(double sampleRate, size_t numChannels, IFactory& factory)
    : pitchShifters_()
    {
        for (size_t n = 0; n < numChannels; ++n)
            pitchShifters_.emplace_back(factory.createPitchShifter(sampleRate, factory));
    }

    void PitchShifterManager::process(core::IAudioBuffer& inputBuffer)
    {
        for (size_t n = 0; n < inputBuffer.getNumChannels(); ++n)
        {
            pitchShifters_[n]->setPitchRatio(pitchRatio_);
            pitchShifters_[n]->process(*inputBuffer.getChannel(n));
        }
    }

    void PitchShifterManager::setPitchRatio(float pitchRatio)
    {
        pitchRatio_ = pitchRatio;
    }
}
