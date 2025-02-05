#include "CircularBuffer.h"


namespace hidonash
{
    CircularBuffer::CircularBuffer(size_t capacity)
    : capacity_(capacity)
    , buffer_(std::make_unique<float[]>(capacity))
    , readIndex_(0)
    , writeIndex_(0)
    , size_(0)
    {}

    void CircularBuffer::push(float value)
    {
        buffer_[writeIndex_] = value;
        writeIndex_ = (writeIndex_ + 1) % capacity_;

        if (size_ < capacity_)
            ++size_;
        else
            readIndex_ = (readIndex_ + 1) % capacity_;
    }

    float CircularBuffer::pop()
    {
        if (isEmpty())
            throw std::runtime_error("Buffer is empty");

        auto value = buffer_[readIndex_];
        readIndex_ = (readIndex_ + 1) % capacity_;
        --size_;

        return value;
    }

    void CircularBuffer::clear()
    {
        readIndex_ = 0;
        writeIndex_ = 0;
        size_ = 0;
    }

    size_t CircularBuffer::size() const
    {
        return size_;
    }

    bool CircularBuffer::isEmpty() const
    {
        return size_ == 0;
    }

    bool CircularBuffer::isFull() const
    {
        return size_ == capacity_;
    }
}
