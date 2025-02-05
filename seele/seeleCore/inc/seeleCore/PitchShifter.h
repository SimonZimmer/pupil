#pragma once

#include <array>

#include <juce_dsp/juce_dsp.h>

#include <core/IAudioBuffer.h>

#include "CircularBuffer.h"
#include "Config.h"
#include "IFactory.h"


namespace hidonash
{
    class PitchShifter : public IPitchShifter
    {
    public:
        PitchShifter(double sampleRate, IFactory& factory);

        PitchShifter& operator=(PitchShifter&& other);

        void process(core::IAudioBuffer::IChannel& channel) override;

        void setPitchRatio(float pitchRatio) override;

    private:
        void processFFTFrame();
        void accumulateOutputWithOverlap();

        const float freqPerBin_;
        IFactory& factory_;
        std::unique_ptr<ISynthesis> synthesis_;
        float pitchFactor_;
        const float gainCompensation_;
        const size_t stepSize_;
        const size_t inFifoLatency_;
        std::unique_ptr<juce::dsp::FFT> fft_;

        CircularBuffer inputBuffer_;
        CircularBuffer outputBuffer_;
        CircularBuffer processingBuffer_;

        std::array<std::complex<float>, config::constants::fftFrameSize> fftWorkspace_;
    };
}
