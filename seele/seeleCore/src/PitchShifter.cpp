#include <cmath>

#include "Config.h"
#include "PitchShifter.h"
#include "core/IAudioBuffer.h"


namespace hidonash
{
    using namespace config;

    namespace
    {
        float getWindowFactor(int k, int windowSize)
        {
            return (-0.5f * std::cos(2.0f * constants::pi * static_cast<float>(k) / static_cast<float>(windowSize)) +
                    0.5f);
        }
    }

    PitchShifter::PitchShifter(double sampleRate, IFactory& factory)
    : freqPerBin_(static_cast<int>(sampleRate / static_cast<double>(constants::fftFrameSize)))
    , factory_(factory)
    , synthesis_(factory_.createSynthesis(freqPerBin_, factory_.createAnalysis(freqPerBin_)))
    , pitchFactor_(1.0f)
    , fft_(std::make_unique<juce::dsp::FFT>(static_cast<int>(std::log2(constants::fftFrameSize))))
    , sampleCounter_(0)
    {
        fifoIn_.fill(0.0f);
        fifoOut_.fill(0.0f);
        outputAccumulationBuffer_.fill(0.0f);
        fftWorkspace_.fill({0.0f, 0.0f});
    }

    void PitchShifter::process(core::IAudioBuffer::IChannel& channel)
    {
        const auto numSamples = channel.size();

        for (auto sa = 0; sa < numSamples; sa++)
        {
            if (sampleCounter_ < fifoIn_.size())
                fifoIn_[sampleCounter_] = channel[sa];

            const auto outputIndex =
                (sampleCounter_ >= constants::inFifoLatency) ? (sampleCounter_ - constants::inFifoLatency) : 0;

            if (outputIndex < fifoOut_.size())
                channel[sa] = fifoOut_[outputIndex];
            else
                channel[sa] = 0.0f;

            sampleCounter_++;

            if (sampleCounter_ >= constants::fftFrameSize)
            {
                for (auto sa = 0; sa < constants::fftFrameSize; ++sa)
                {
                    fftWorkspace_[sa].real(fifoIn_[sa] * getWindowFactor(sa, constants::fftFrameSize));
                    fftWorkspace_[sa].imag(0.0f);
                }

                fft_->perform(fftWorkspace_.data(), fftWorkspace_.data(), false);
                synthesis_->perform(fftWorkspace_.data(), pitchFactor_);
                fft_->perform(fftWorkspace_.data(), fftWorkspace_.data(), true);

                overlapAdd();
            }
        }
    }

    void PitchShifter::overlapAdd()
    {
        for (auto sa = 0; sa < constants::fftFrameSize; ++sa)
            outputAccumulationBuffer_[sa] +=
                2.0f * getWindowFactor(sa, constants::fftFrameSize) * fftWorkspace_[sa].real();


        std::copy(outputAccumulationBuffer_.data(), outputAccumulationBuffer_.data() + constants::stepSize,
                  fifoOut_.data());

        std::copy(outputAccumulationBuffer_.begin() + constants::stepSize,
                  outputAccumulationBuffer_.begin() + constants::stepSize + constants::fftFrameSize,
                  outputAccumulationBuffer_.begin());

        std::copy(fifoIn_.data() + constants::stepSize, fifoIn_.data() + constants::stepSize + constants::inFifoLatency,
                  fifoIn_.data());

        sampleCounter_ = constants::inFifoLatency;
    }

    void PitchShifter::setPitchRatio(float pitchRatio)
    {
        pitchFactor_ = pitchRatio;
    }
}
