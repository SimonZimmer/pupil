#include <memory>

namespace hidonash
{
    class CircularBuffer
    {
    public:
        explicit CircularBuffer(size_t capacity);

        void push(float value);

        float pop();

        float peek(size_t offset = 0) const;

        void clear();

        size_t size() const;

        bool isEmpty() const;

        bool isFull() const;

    private:
        size_t capacity_;
        std::unique_ptr<float[]> buffer_;
        size_t readIndex_;
        size_t writeIndex_;
        size_t size_;
    };
}
