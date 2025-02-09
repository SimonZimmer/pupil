#include <cmath>

#include "Config.h"
#include "Synthesis.h"


namespace hidonash
{
    using namespace config;

    Synthesis::Synthesis(int freqPerBin, AnalysisPtr analysis)
    : analysis_(std::move(analysis))
    , freqPerBin_(freqPerBin)
    {}

    void Synthesis::perform(juce::dsp::Complex<float>* fftWorkspace, float pitchFactor)
    {
        auto analysisResult = analysis_->perform(fftWorkspace);

        std::fill(magnitudeBuffer_.begin(), magnitudeBuffer_.end(), 0.0f);
        std::fill(frequencyBuffer_.begin(), frequencyBuffer_.end(), 0.0f);

        const auto analysisMagnitudeBuffer = analysisResult.magnitudeBuffer;
        const auto analysisFrequencyBuffer = analysisResult.frequencyBuffer;

        const int halfFrameSize = constants::fftFrameSize / 2;

        for (auto sa = 0; sa <= halfFrameSize; ++sa)
        {
            const float targetIndex = static_cast<float>(sa) * pitchFactor;
            const int index = static_cast<int>(std::floor(targetIndex));

            if (index >= 0 && index <= halfFrameSize)
            {
                float weight = targetIndex - index;
                magnitudeBuffer_[index] += analysisMagnitudeBuffer[sa] * (1.0f - weight);

                if (index + 1 <= halfFrameSize)
                    magnitudeBuffer_[index + 1] += analysisMagnitudeBuffer[sa] * weight;

                frequencyBuffer_[index] = analysisFrequencyBuffer[sa] * pitchFactor;
            }
        }

        const float twoPiOverOSF = 2.0f * constants::pi / static_cast<float>(constants::oversamplingFactor);
        const float expectedPhaseIncrement = config::constants::expectedPhaseDifference;

        for (auto sa = 0; sa <= constants::fftFrameSize; ++sa)
        {
            const float magnitude = magnitudeBuffer_[sa];
            float phase = frequencyBuffer_[sa];

            const float saDouble = static_cast<float>(sa);
            float phaseDifference = (phase - saDouble * freqPerBin_) / freqPerBin_;
            phaseDifference = phaseDifference * twoPiOverOSF + saDouble * expectedPhaseIncrement;

            sumPhase_[sa] += phaseDifference;
            phase = sumPhase_[sa];

            fftWorkspace[sa] = std::polar(magnitude, phase);
        }
    }
}
