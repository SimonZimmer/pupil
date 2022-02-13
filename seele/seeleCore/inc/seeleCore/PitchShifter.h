#pragma once

#include <array>

#include <juce_dsp/juce_dsp.h>

#include <core/AudioBuffer.h>

#include "IAudioProcessor.h"

namespace hidonash
{
    class PitchShifter
    {
    public:
        explicit PitchShifter(double sampleRate);
        ~PitchShifter() = default;

        void process(core::AudioBuffer<float>& audioBuffer);

        void setPitchRatio(float pitchRatio);

        void setFftFrameSize(float fftFrameSize);

        void fft(float *fftBuffer, long fftFrameSize, bool inverse);

    private:
        float pitchFactor_{ 0.f };
        int max_frame_length_{ 8192 };
        double sampleRate_;
        size_t fftFrameSize_;

        std::array<float, 8192> inFifo_;
        std::array<float, 8192> outFifo_;
        std::array<float, 2*8192> fftWorkspace_;
        std::array<float, 8192/2+1> lastPhase_;
        std::array<float, 8192/2+1> sumPhase_;
        std::array<float, 2*8192> outputAccumulator_;
        std::array<float, 8192> gAnaFreq;
        std::array<float, 8192> gAnaMagn;
        std::array<float, 8192> synthesisFrequencyBuffer_;
        std::array<float, 8192> synthesisMagnitude_;

        std::vector<juce::dsp::Complex<float>> buffer_;
        std::unique_ptr<juce::dsp::FFT> fft_;
        std::vector<double> window_;
    };

}
