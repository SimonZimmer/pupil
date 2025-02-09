#pragma once

#include <gmock/gmock.h>

#include <seeleCore/IAnalysis.h>


namespace hidonash
{
    class AnalysisMock : public IAnalysis
    {
    public:
        ~AnalysisMock() override = default;

        MOCK_METHOD(AnalysisResult, perform, (juce::dsp::Complex<float>*) );
    };
}
