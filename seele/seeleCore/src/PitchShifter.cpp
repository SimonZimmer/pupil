#include <cmath>

#include "Config.h"
#include "PitchShifter.h"
#include "core/IAudioBuffer.h"


namespace hidonash
{
    using namespace config;

    namespace
    {
        double getWindowFactor(size_t k, size_t windowSize)
        {
            return (-.5 * cos(2. * M_PI * (double) k / (double) windowSize) + .5);
        }
    }

    PitchShifter::PitchShifter(double sampleRate, IFactory& factory)
    : freqPerBin_(static_cast<int>(sampleRate / static_cast<double>(constants::fftFrameSize)))
    , factory_(factory)
    , synthesis_(factory_.createSynthesis(freqPerBin_, factory_.createAnalysis(freqPerBin_)))
    , pitchFactor_(1.0f)
    , gainCompensation_(std::pow(10, (65. / 20.)))
    , sampleCounter_(0)
    , stepSize_(config::constants::fftFrameSize / config::constants::oversamplingFactor)
    , inFifoLatency_(config::constants::fftFrameSize - stepSize_)
    , fft_(std::make_unique<juce::dsp::FFT>(static_cast<int>(std::log2(constants::fftFrameSize))))
    {
        fifoIn_.fill(0.0f);
        fifoOut_.fill(0.0f);
        outputAccumulationBuffer_.fill(0.0f);
    }

    void PitchShifter::process(core::IAudioBuffer::IChannel& channel)
    {
        const auto numSamples = channel.size();

        for (auto sa = 0; sa < numSamples; sa++)
        {
            fifoIn_[sampleCounter_] = channel[sa];
            channel[sa] = fifoOut_[sampleCounter_ - inFifoLatency_];
            sampleCounter_++;

            if (sampleCounter_ >= constants::fftFrameSize)
                performFFTProcessing();
        }

        channel.applyGain(gainCompensation_);
    }

    void PitchShifter::performFFTProcessing()
    {
        for (auto sa = 0; sa < constants::fftFrameSize; sa++)
        {
            fftWorkspace_[sa].real(fifoIn_[sa] * getWindowFactor(sa, constants::fftFrameSize));
            fftWorkspace_[sa].imag(0.);
        }

        fft_->perform(fftWorkspace_.data(), fftWorkspace_.data(), false);
        synthesis_->perform(fftWorkspace_.data(), pitchFactor_);
        fft_->perform(fftWorkspace_.data(), fftWorkspace_.data(), true);

        for (auto sa = 0; sa < constants::fftFrameSize; sa++)
            outputAccumulationBuffer_[sa] += 2. * getWindowFactor(sa, constants::fftFrameSize) *
                                             fftWorkspace_[sa].real() /
                                             ((constants::fftFrameSize / 2) * constants::oversamplingFactor);

        for (auto sa = 0; sa < stepSize_; sa++)
            fifoOut_[sa] = outputAccumulationBuffer_[sa];

        memmove(outputAccumulationBuffer_.data(), outputAccumulationBuffer_.data() + stepSize_,
                constants::fftFrameSize * sizeof(float));

        for (auto sa = 0; sa < inFifoLatency_; sa++)
            fifoIn_[sa] = fifoIn_[sa + stepSize_];

        sampleCounter_ = inFifoLatency_;
    }

    void PitchShifter::setPitchRatio(float pitchRatio)
    {
        pitchFactor_ = pitchRatio;
    }
}
