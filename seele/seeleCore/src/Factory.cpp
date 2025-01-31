#include "Factory.h"

#include <core/AudioBuffer.h>
#include <memory>

#include "Analysis.h"
#include "DelayProcessor.h"
#include "Engine.h"
#include "GainProcessor.h"
#include "PitchShifter.h"
#include "PitchShifterManager.h"
#include "Synthesis.h"


namespace hidonash
{
    AudioProcessorPtr Factory::createEngine(const IMemberParameterSet& memberParameterSet, double sampleRate,
                                            int samplesPerBlock, size_t numChannels) const
    {
        return std::make_unique<Engine>(memberParameterSet, sampleRate, samplesPerBlock, numChannels);
    }

    AnalysisPtr Factory::createAnalysis(int freqPerBin) const
    {
        return std::make_unique<Analysis>(freqPerBin);
    }

    SynthesisPtr Factory::createSynthesis(int freqPerBin, AnalysisPtr analysis) const
    {
        return std::make_unique<Synthesis>(freqPerBin, std::move(analysis));
    }

    PitchShifterPtr Factory::createPitchShifter(double sampleRate, IFactory& factory) const
    {
        return std::make_unique<PitchShifter>(sampleRate, factory);
    }

    PitchShifterManagerPtr Factory::createPitchShifterManager(double sampleRate, size_t numChannels,
                                                              IFactory& factory) const
    {
        return std::make_unique<PitchShifterManager>(sampleRate, numChannels, factory);
    }

    DelayProcessorPtr Factory::createDelayProcessor(size_t maxDelaySeconds, size_t delaySeconds,
                                                    double sampleRate) const
    {
        return std::make_unique<DelayProcessor>(maxDelaySeconds, delaySeconds, sampleRate);
    }

    GainProcessorPtr Factory::createGainProcessor(float gainValue, double sampleRate) const
    {
        return std::make_unique<GainProcessor>(gainValue, sampleRate);
    }

    core::AudioBufferPtr Factory::createAudioBuffer(int numChannels, int numSamples) const
    {
        return std::make_unique<core::AudioBuffer>(numChannels, numSamples);
    }
}
