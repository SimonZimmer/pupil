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

        constexpr float gainCompensationDb = 65.0f;
        constexpr float dbConversionFactor = 20.0f;
    }

    PitchShifter::PitchShifter(double sampleRate, IFactory& factory)
    : freqPerBin_(static_cast<int>(sampleRate / static_cast<double>(constants::fftFrameSize)))
    , factory_(factory)
    , synthesis_(factory_.createSynthesis(freqPerBin_, factory_.createAnalysis(freqPerBin_)))
    , pitchFactor_(1.0f)
    , gainCompensation_(std::pow(10, (gainCompensationDb / dbConversionFactor)))
    , stepSize_(config::constants::fftFrameSize / config::constants::oversamplingFactor)
    , inFifoLatency_(config::constants::fftFrameSize - stepSize_)
    , fft_(std::make_unique<juce::dsp::FFT>(static_cast<int>(std::log2(constants::fftFrameSize))))
    , inputBuffer_(constants::fftFrameSize)
    , outputBuffer_(constants::fftFrameSize)
    , processingBuffer_(constants::fftFrameSize)
    {}

    void PitchShifter::processFFTFrame()
    {
        // Copy input samples to FFT workspace with windowing
        for (size_t sa = 0; sa < constants::fftFrameSize; ++sa)
        {
            auto windowedSample = inputBuffer_.pop() * getWindowFactor(sa, constants::fftFrameSize);
            fftWorkspace_[sa] = std::complex<float>(windowedSample, 0.0f);
        }

        // Perform FFT and pitch shifting
        fft_->perform(fftWorkspace_.data(), fftWorkspace_.data(), false);
        synthesis_->perform(fftWorkspace_.data(), pitchFactor_);
        fft_->perform(fftWorkspace_.data(), fftWorkspace_.data(), true);

        // Accumulate output with overlap-add method
        accumulateOutputWithOverlap();
    }

    void PitchShifter::accumulateOutputWithOverlap()
    {
        // Accumulate output with windowing and normalization
        for (size_t sa = 0; sa < constants::fftFrameSize; ++sa)
        {
            float accumulatedSample = getWindowFactor(sa, constants::fftFrameSize) * fftWorkspace_[sa].real() /
                                      ((constants::fftFrameSize / 2) * constants::oversamplingFactor);
            processingBuffer_.push(accumulatedSample);
        }

        // Push first stepSize samples to output buffer
        for (size_t sa = 0; sa < stepSize_; ++sa)
            outputBuffer_.push(processingBuffer_.pop());
    }

    void PitchShifter::process(core::IAudioBuffer::IChannel& channel)
    {
        const auto numSamples = channel.size();

        for (size_t sa = 0; sa < numSamples; ++sa)
        {
            // Push input sample to input buffer
            inputBuffer_.push(channel[sa]);

            // Check if we have enough samples to process an FFT frame
            if (inputBuffer_.size() >= constants::fftFrameSize)
                processFFTFrame();

            // Read processed output
            channel[sa] = outputBuffer_.pop();
        }

        // Apply gain compensation
        channel.applyGain(gainCompensation_);
    }

    void PitchShifter::setPitchRatio(float pitchRatio)
    {
        pitchFactor_ = pitchRatio;
    }
}
