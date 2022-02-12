#include <cmath>

#include "PitchShifter.h"
#include "Config.h"

namespace hidonash
{
    PitchShifter::PitchShifter(double sampleRate)
    : sampleRate_(sampleRate)
    {
    }

    void PitchShifter::process(core::AudioBuffer<float>& audioBuffer)
    {
        static long gRover = false, gInit = false;
        double magn, phase, tmp, window, real, imag;
        double freqPerBin, expct;
        long i,k, qpd, index, inFifoLatency, stepSize, fftFrameSize2;

        /* set up some handy variables */
        fftFrameSize2 = fftFrameSize_/2;
        stepSize = fftFrameSize_/config::constants::oversamplingFactor;
        freqPerBin = sampleRate_/(double)fftFrameSize_;
        expct = 2.*M_PI*(double)stepSize/(double)fftFrameSize_;
        inFifoLatency = fftFrameSize_-stepSize;
        if (gRover == false) gRover = inFifoLatency;

        /* initialize our arrays */
        if (gInit == false)
        {
            std::memset(gInFIFO.data(), 0, max_frame_length_*sizeof(float));
            std::memset(gOutFIFO.data(), 0, max_frame_length_*sizeof(float));
            std::memset(gFFTworksp.data(), 0, 2*max_frame_length_*sizeof(float));
            std::memset(gLastPhase.data(), 0, (max_frame_length_/2+1)*sizeof(float));
            std::memset(gSumPhase.data(), 0, (max_frame_length_/2+1)*sizeof(float));
            std::memset(gOutputAccum.data(), 0, 2*max_frame_length_*sizeof(float));
            std::memset(gAnaFreq.data(), 0, max_frame_length_*sizeof(float));
            std::memset(gAnaMagn.data(), 0, max_frame_length_*sizeof(float));
            gInit = true;
        }

        /* main processing loop */
        auto indata = audioBuffer.getDataPointer();
        auto outdata = audioBuffer.getDataPointer();
        for (i = 0; i < audioBuffer.getNumSamples(); i++)
        {
            /* As long as we have not yet collected enough data just read in */
            gInFIFO[gRover] = indata[i];
            outdata[i] = gOutFIFO[gRover-inFifoLatency];
            gRover++;

            /* now we have enough data for processing */
            if (gRover >= fftFrameSize_)
            {
                gRover = inFifoLatency;

                /* do windowing and re,im interleave */
                for (k = 0; k < fftFrameSize_;k++)
                {
                    window = -.5*cos(2.*M_PI*(double)k/(double)fftFrameSize_)+.5;
                    gFFTworksp[2*k] = gInFIFO[k] * window;
                    gFFTworksp[2*k+1] = 0.;
                }

                /* ***************** ANALYSIS ******************* */
                /* do transform */
                smbFft(gFFTworksp.data(), fftFrameSize_, -1);

                /* this is the analysis step */
                for (k = 0; k <= fftFrameSize2; k++)
                {
                    /* de-interlace FFT buffer */
                    real = gFFTworksp[2*k];
                    imag = gFFTworksp[2*k+1];

                    /* compute magnitude and phase */
                    magn = 2.*sqrt(real*real + imag*imag);
                    phase = atan2(imag,real);

                    /* compute phase difference */
                    tmp = phase - gLastPhase[k];
                    gLastPhase[k] = phase;

                    /* subtract expected phase difference */
                    tmp -= (double)k*expct;

                    /* map delta phase into +/- Pi interval */
                    qpd = tmp/M_PI;
                    if (qpd >= 0) qpd += qpd&1;
                    else qpd -= qpd&1;
                    tmp -= M_PI*(double)qpd;

                    /* get deviation from bin frequency from the +/- Pi interval */
                    tmp = config::constants::oversamplingFactor*tmp/(2.*M_PI);

                    /* compute the k-th partials' true frequency */
                    tmp = (double)k*freqPerBin + tmp*freqPerBin;

                    /* store magnitude and true frequency in analysis arrays */
                    gAnaMagn[k] = magn;
                    gAnaFreq[k] = tmp;
                }

                /* ***************** PROCESSING ******************* */
                /* this does the actual pitch shifting */
                memset(gSynMagn.data(), 0, fftFrameSize_*sizeof(float));
                memset(gSynFreq.data(), 0, fftFrameSize_*sizeof(float));
                for (k = 0; k <= fftFrameSize2; k++)
                {
                    index = k * pitchFactor_;
                    if (index <= fftFrameSize2)
                    {
                        gSynMagn[index] += gAnaMagn[k];
                        gSynFreq[index] = gAnaFreq[k] * pitchFactor_;
                    }
                }

                /* ***************** SYNTHESIS ******************* */
                /* this is the synthesis step */
                for (k = 0; k <= fftFrameSize2; k++)
                {
                    /* get magnitude and true frequency from synthesis arrays */
                    magn = gSynMagn[k];
                    tmp = gSynFreq[k];

                    /* subtract bin mid frequency */
                    tmp -= (double)k*freqPerBin;

                    /* get bin deviation from freq deviation */
                    tmp /= freqPerBin;

                    /* take osamp into account */
                    tmp = 2.*M_PI*tmp/config::constants::oversamplingFactor;

                    /* add the overlap phase advance back in */
                    tmp += (double)k*expct;

                    /* accumulate delta phase to get bin phase */
                    gSumPhase[k] += tmp;
                    phase = gSumPhase[k];

                    /* get real and imag part and re-interleave */
                    gFFTworksp[2*k] = magn*cos(phase);
                    gFFTworksp[2*k+1] = magn*sin(phase);
                }

                /* zero negative frequencies */
                for (k = fftFrameSize_+2; k < 2*fftFrameSize_; k++) gFFTworksp[k] = 0.;

                /* do inverse transform */
                smbFft(gFFTworksp.data(), fftFrameSize_, 1);

                /* do windowing and add to output accumulator */
                for(k=0; k < fftFrameSize_; k++)
                {
                    window = -.5*cos(2.*M_PI*(double)k/(double)fftFrameSize_)+.5;
                    gOutputAccum[k] += 2.*window*gFFTworksp[2*k]/(fftFrameSize2*config::constants::oversamplingFactor);
                }
                for (k = 0; k < stepSize; k++) gOutFIFO[k] = gOutputAccum[k];

                /* shift accumulator */
                memmove(gOutputAccum.data(), gOutputAccum.data() + stepSize, fftFrameSize_*sizeof(float));

                /* move input FIFO */
                for (k = 0; k < inFifoLatency; k++) gInFIFO[k] = gInFIFO[k+stepSize];
            }
        }
    }

    void PitchShifter::smbFft(float* fftBuffer, long fftFrameSize, long sign)
    /*
        FFT routine, (C)1996 S.M.Bernsee. Sign = -1 is FFT, 1 is iFFT (inverse)
        Fills fftBuffer[0...2*fftFrameSize-1] with the Fourier transform of the
        time domain data in fftBuffer[0...2*fftFrameSize-1]. The FFT array takes
        and returns the cosine and sine parts in an interleaved manner, ie.
        fftBuffer[0] = cosPart[0], fftBuffer[1] = sinPart[0], asf. fftFrameSize
        must be a power of 2. It expects a complex input signal (see footnote 2),
        ie. when working with 'common' audio signals our input signal has to be
        passed as {in[0],0.,in[1],0.,in[2],0.,...} asf. In that case, the transform
        of the frequencies of interest is in fftBuffer[0...fftFrameSize].
    */
    {
        for (auto k = 0; k < 2*fftFrameSize;k++)
        {
            buffer_[k].real(fftBuffer[2*k]);
            buffer_[k].imag(fftBuffer[2*k+1]);
        }

        if(sign == -1)
            fft_->perform(buffer_.data(), buffer_.data(), false);
        else
            fft_->perform(buffer_.data(), buffer_.data(), true);

        for (auto k = 0; k < (2*fftFrameSize);k++)
        {
            fftBuffer[2*k] = buffer_[k].real();
            fftBuffer[2*k+1] = buffer_[k].imag();
        }
    }

    void PitchShifter::setPitchRatio(float pitchRatio)
    {
        pitchFactor_ = pitchRatio;
    }

    void PitchShifter::setFftFrameSize(float fftFrameSize)
    {
        const auto fftFrameSizeIndex = static_cast<int>(fftFrameSize) % config::parameters::fftFrameSizeChoices.size();
        fftFrameSize_ = config::parameters::fftFrameSizeChoices[fftFrameSizeIndex];

        buffer_.resize(2*fftFrameSize_);

        const auto fftOrder = std::log2(fftFrameSize_);
        fft_ = std::make_unique<juce::dsp::FFT>(static_cast<int>(fftOrder));
    }
}g
