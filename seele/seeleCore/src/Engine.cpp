#include "Engine.h"

#include "IGainProcessor.h"
#include "core/AudioBuffer.h"

#include "Config.h"
#include "IMemberParameterSet.h"


namespace hidonash
{
    Engine::Engine(const IMemberParameterSet& memberParameterSet, double sampleRate, int samplesPerBlock,
                   size_t numChannels, FactoryPtr factory)
    : memberParameterSet_(memberParameterSet)
    , accumulationBuffer_(numChannels, samplesPerBlock)
    , numChannels_(numChannels)
    {
        delayProcessors_.resize(numChannels_);
        gainProcessors_.resize(numChannels_);
        for (auto n = 0; n < config::constants::numMembers; ++n)
        {
            pitchShifterManagers_.emplace_back(factory->createPitchShifterManager(sampleRate, numChannels_, *factory));
            audioBuffers_.emplace_back(factory->createAudioBuffer(numChannels_, samplesPerBlock));
            for (size_t ch = 0; ch < numChannels_; ++ch)
            {
                delayProcessors_[ch].emplace_back(
                    factory->createDelayProcessor(config::parameters::maxDistanceInSeconds, 0.0f, sampleRate));
                gainProcessors_[ch].emplace_back(factory->createGainProcessor(0.0f, sampleRate));
            }
        }
    }

    void Engine::process(core::IAudioBuffer& inputBuffer)
    {
        accumulationBuffer_.fill(0.0f);

        int activeMembers = 0;
        for (size_t n = 0; n < config::constants::numMembers; ++n)
            if (memberParameterSet_.getSummonState(n))
            {
                audioBuffers_[n]->copyFrom(inputBuffer);
                pitchShifterManagers_[n]->setPitchRatio(memberParameterSet_.getSanctity(n));
                pitchShifterManagers_[n]->process(*audioBuffers_[n]);

                for (size_t ch = 0; ch < numChannels_; ++ch)
                {
                    delayProcessors_[ch][n]->setDelayInSeconds(memberParameterSet_.getDistance(n));
                    delayProcessors_[ch][n]->process(*audioBuffers_[n]->getChannel(ch));
                    gainProcessors_[ch][n]->setGainDb(memberParameterSet_.getGain(n));
                    gainProcessors_[ch][n]->process(*audioBuffers_[n]->getChannel(ch));
                }

                accumulationBuffer_.add(*audioBuffers_[n], inputBuffer.getNumSamples());
                activeMembers++;
            }

        if (activeMembers <= 0)
            return;

        accumulationBuffer_.multiply(1.f / static_cast<float>(activeMembers), accumulationBuffer_.getNumSamples());

        inputBuffer.copyFrom(accumulationBuffer_);
    }
}
