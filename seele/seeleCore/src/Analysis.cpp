#include <cmath>

#include "Analysis.h"
#include "Config.h"


namespace hidonash
{
    namespace
    {
        int roundToNearestEvenNumber(int value)
        {
            if (value >= 0)
                value += value & 1;
            else
                value -= value & 1;

            return value;
        }
    }

    using namespace config;

    Analysis::Analysis(int freqPerBin)
    : freqPerBin_(freqPerBin)
    {
        std::fill(lastPhase_.begin(), lastPhase_.end(), 0.0f);
    }

    AnalysisResult Analysis::perform(juce::dsp::Complex<float>* fftWorkspace)
    {
        for (auto sa = 0; sa <= config::constants::fftFrameSize / 2; sa++)
        {
            const auto real = fftWorkspace[sa].real();
            const auto imag = fftWorkspace[sa].imag();
            const auto magnitude = 2. * sqrt(real * real + imag * imag);
            const auto phase = atan2(imag, real);
            auto phaseDifference = phase - lastPhase_[sa];
            lastPhase_[sa] = phase;
            phaseDifference -= static_cast<double>(sa) * constants::expectedPhaseDifference;
            long phaseDifferenceQuotient = phaseDifference / constants::pi;
            phaseDifferenceQuotient = roundToNearestEvenNumber(phaseDifferenceQuotient);
            phaseDifference -= constants::pi * static_cast<double>(phaseDifferenceQuotient);
            phaseDifference = constants::oversamplingFactor * phaseDifference / (2. * constants::pi);
            phaseDifference = static_cast<double>(sa) * freqPerBin_ + phaseDifference * freqPerBin_;

            analysisMagnitudeBuffer_[sa] = magnitude;
            analysisFrequencyBuffer_[sa] = phaseDifference;
        }

        return AnalysisResult {analysisMagnitudeBuffer_, analysisFrequencyBuffer_};
    }
}
