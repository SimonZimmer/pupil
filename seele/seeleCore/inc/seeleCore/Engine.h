#pragma once

#include <vector>

#include <core/AudioBuffer.h>

#include "Factory.h"
#include "IDelayProcessor.h"
#include "IFactory.h"
#include "IGainProcessor.h"
#include "IMemberParameterSet.h"
#include "IPitchShifterManager.h"


namespace hidonash
{
    class Engine : public IAudioProcessor
    {
    public:
        Engine(const IMemberParameterSet& memberParameterSet, double sampleRate, int samplesPerBlock,
               size_t numChannels, FactoryPtr factory = std::make_unique<Factory>());

        ~Engine() = default;

        void process(core::IAudioBuffer& inputBuffer) override;

    private:
        std::vector<PitchShifterManagerPtr> pitchShifterManagers_;
        std::vector<std::vector<DelayProcessorPtr>> delayProcessors_;
        std::vector<std::vector<GainProcessorPtr>> gainProcessors_;
        std::vector<core::AudioBufferPtr> audioBuffers_;
        const IMemberParameterSet& memberParameterSet_;
        core::AudioBuffer accumulationBuffer_;
        size_t numChannels_;
        double sampleRate_;
        std::vector<float> lastDistances_;
    };
}
