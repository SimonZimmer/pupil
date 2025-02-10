#pragma once

#include <array>
#include <memory>

#include <juce_dsp/juce_dsp.h>

#include "Config.h"

namespace hidonash
{
    struct AnalysisResult
    {
        std::array<float, config::constants::fftFrameSize> magnitudeBuffer;
        std::array<float, config::constants::fftFrameSize> frequencyBuffer;
    };

    class IAnalysis
    {
    public:
        virtual ~IAnalysis() = default;

        virtual AnalysisResult perform(juce::dsp::Complex<float>* fftWorkspace) = 0;
    };

    using AnalysisPtr = std::unique_ptr<IAnalysis>;
}
