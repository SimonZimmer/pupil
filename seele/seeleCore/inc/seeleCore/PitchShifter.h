#pragma once

#include <array>

#include <juce_dsp/juce_dsp.h>

#include <core/IAudioBuffer.h>

#include "Config.h"
#include "IFactory.h"


namespace hidonash
{
    class PitchShifter : public IPitchShifter
    {
    public:
        PitchShifter(double sampleRate, IFactory& factory);

        void process(core::IAudioBuffer::IChannel& channel) override;

        void setPitchRatio(float pitchRatio) override;

    private:
        void overlapAdd();

        int freqPerBin_;
        IFactory& factory_;
        SynthesisPtr synthesis_;

        float pitchFactor_;

        std::unique_ptr<juce::dsp::FFT> fft_;
        std::array<juce::dsp::Complex<float>, config::constants::fftFrameSize> fftWorkspace_;

        std::array<float, config::constants::fftFrameSize> fifoIn_;
        std::array<float, config::constants::fftFrameSize> fifoOut_;
        std::array<float, config::constants::fftFrameSize> outputAccumulationBuffer_;

        size_t sampleCounter_;
    };
}
