#pragma once

#include "IAudioProcessor.h"
#include "IFactory.h"


namespace hidonash
{
    class Factory : public IFactory
    {
    public:
        ~Factory() override = default;

        AudioProcessorPtr createEngine(const IMemberParameterSet& memberParameterSet, double sampleRate,
                                       int samplesPerBlock, size_t numChannels) const override;

        [[nodiscard]] AnalysisPtr createAnalysis(int freqPerBin) const override;

        [[nodiscard]] SynthesisPtr createSynthesis(int freqPerBin, AnalysisPtr analysis) const override;

        [[nodiscard]] PitchShifterPtr createPitchShifter(double sampleRate, IFactory& factory) const override;

        [[nodiscard]] DelayProcessorPtr createDelayProcessor(size_t maxDelaySeconds, size_t delaySeconds,
                                                             double sampleRate) const override;

        [[nodiscard]] GainProcessorPtr createGainProcessor(float gainValue, double sampleRate) const override;

        [[nodiscard]] PitchShifterManagerPtr createPitchShifterManager(double sampleRate, size_t numChannels,
                                                                       IFactory& factory) const override;

        [[nodiscard]] core::AudioBufferPtr createAudioBuffer(int numChannels, int numSamples) const override;
    };
}
