#pragma once

#include <juce_dsp/juce_dsp.h>
#include <memory>


namespace hidonash
{
    class ISynthesis
    {
    public:
        virtual ~ISynthesis() = default;

        virtual void perform(juce::dsp::Complex<float>* fftWorkspace, float pitchFactor) = 0;
    };

    using SynthesisPtr = std::unique_ptr<ISynthesis>;
}
