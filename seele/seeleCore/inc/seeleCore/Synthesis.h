#pragma once

#include "Config.h"
#include "IAnalysis.h"
#include "ISynthesis.h"


namespace hidonash
{
    class Synthesis : public ISynthesis
    {
    public:
        explicit Synthesis(int freqPerBin, AnalysisPtr analysis);

        ~Synthesis() override = default;

        void perform(juce::dsp::Complex<float>* fftWorkspace, float pitchFactor) override;

    private:
        AnalysisPtr analysis_;
        int freqPerBin_;

        std::array<float, config::constants::analysisSize / 2 + 1> sumPhase_;
        std::array<float, config::constants::analysisSize> frequencyBuffer_;
        std::array<float, config::constants::analysisSize> magnitudeBuffer_;
    };

}
