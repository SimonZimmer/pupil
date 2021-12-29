#pragma once

#include <memory>

#include <core/AudioBuffer.h>

namespace sz
{
    class BlockCircularBuffer
    {
    public:
        BlockCircularBuffer(long newSize)
        {
            setSize(newSize, true);
        }

        void setReadHopSize(int hopSize)
        {
            readHopSize = hopSize;
        }

        void setWriteHopSize(int hopSize)
        {
            writeHopSize = hopSize;
        }

        void setSize(long newSize, bool shouldClear = false)
        {
            if (newSize == length)
            {
                if (shouldClear)
                    reset ();

                return;
            }

            block.setSize(1, newSize);
            length = newSize;
            writeIndex = readIndex = 0;
        }

        void reset()
        {
            block.fill(0.f);
            writeIndex = readIndex = 0;
        }

        // Read samples from the internal buffer into the 'destBuffer'
        // perform a wrap of the read if near the internal buffer boundaries
        void read(core::AudioBuffer<float>& destBuffer, int offset, const long destLength)
        {
            const auto firstReadAmount = readIndex + destLength >= length ?
                                         length - readIndex : destLength;

            destBuffer.copy(block, readIndex, offset,firstReadAmount);

            if(firstReadAmount < destLength)
                destBuffer.copy(block, 0, firstReadAmount, destLength - firstReadAmount);

            readIndex += readHopSize != 0 ? readHopSize : destLength;
            readIndex %= length;
        }

        // Write all samples from the 'sourceBuffer' into the internal buffer
        // Perform any wrapping required
        void write(core::AudioBuffer<float>& sourceBuffer, const int offset, const long writeLength)
        {
            const auto firstWriteAmount = writeIndex + writeLength >= length ?
                                          length - writeIndex : writeLength;

            block.copy(sourceBuffer, offset, writeIndex, firstWriteAmount);

            if(firstWriteAmount < writeLength)
                block.copy(sourceBuffer, firstWriteAmount + offset, writeIndex, writeLength - firstWriteAmount);

            writeIndex += writeHopSize != 0 ? writeHopSize : writeLength;
            writeIndex %= length;
            latestDataIndex = writeIndex + writeLength % length;
        }

        // The first 'overlapAmount' of 'sourceBuffer' samples are added to the existing buffer
        // The remainder of samples are set in the buffer (overwrite)
        void overlapWrite(core::AudioBuffer<float>& sourceBuffer, const long writeLength)
        {
            auto&& sourceBufferData = sourceBuffer.getDataPointer();
            // Since we're using a circular buffer, we have to be careful when to add samples to the existing
            // data and when to overwrite out of date samples. This number can change when modulating between
            // the pitch (which alters the size of the overlaps). The calculation below will determine the
            // index we need to "add" to and at which point we need to "set" the samples to overwrite the history
            const auto overlapAmount = getOverlapAmount(writeLength);
            auto tempWriteIndex = writeIndex;
            auto firstWriteAmount = writeIndex + overlapAmount > length ? length - writeIndex : overlapAmount;

            auto internalBuffer = block.getDataPointer();

            for(auto i = 0; i < firstWriteAmount; ++i)
                internalBuffer[i + writeIndex] += sourceBufferData[i];

            if(firstWriteAmount < overlapAmount)
                for(auto i = 0; i < (overlapAmount - firstWriteAmount); ++i)
                    internalBuffer[i] += sourceBufferData[i + firstWriteAmount];

            tempWriteIndex += overlapAmount;
            tempWriteIndex %= length;

            const auto remainingElements = writeLength - overlapAmount;
            firstWriteAmount = tempWriteIndex + remainingElements > length ?
                               length - tempWriteIndex : remainingElements;

            block.copy(sourceBuffer, overlapAmount, tempWriteIndex, firstWriteAmount);

            if (firstWriteAmount < remainingElements)
                block.copy(sourceBuffer, overlapAmount + firstWriteAmount, 0, (remainingElements - firstWriteAmount));

            writeIndex += writeHopSize;
            writeIndex %= length;
            latestDataIndex = (writeIndex + writeLength) % length;
        }

    private:
        size_t getOverlapAmount(size_t writeLength)
        {
            const int writeIndexDifference = (writeIndex <= latestDataIndex) ? latestDataIndex - writeIndex : length - writeIndex + latestDataIndex;
            const int overlapSampleCount = writeLength - writeHopSize;

            return std::min(writeIndexDifference, overlapSampleCount);
        }

        core::AudioBuffer<float> block{1, 0};
        long writeIndex = 0;
        long readIndex = 0;
        long length = 0;
        long latestDataIndex = 0;
        int writeHopSize = 0;
        int readHopSize = 0;
    };
}
