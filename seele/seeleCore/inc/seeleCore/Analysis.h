#pragma once

#include "Config.h"
#include "IAnalysis.h"


namespace hidonash
{
    class Analysis : public IAnalysis
    {
    public:
        explicit Analysis(int freqPerBin);

        ~Analysis() override = default;

        AnalysisResult perform(juce::dsp::Complex<float>* fftWorkspace) override;

    private:
        int freqPerBin_;

        std::array<float, config::constants::analysisSize / 2 + 1> lastPhase_;
        std::array<float, config::constants::analysisSize> analysisFrequencyBuffer_;
        std::array<float, config::constants::analysisSize> analysisMagnitudeBuffer_;
    };

}
