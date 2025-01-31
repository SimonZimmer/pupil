#include "InterpolatingValue.h"


namespace hidonash::core
{
    template<typename T>
    InterpolatingValue<T>::InterpolatingValue(T value, double sampleRate, const std::chrono::milliseconds& duration)
    : currentValue_(value)
    , goalValue_(value)
    , durationInTicks_(static_cast<size_t>(static_cast<T>(duration.count()) / 1000.f * sampleRate))
    , step_((goalValue_ - currentValue_) / static_cast<T>(durationInTicks_))
    {}

    template<typename T>
    InterpolatingValue<T>::operator T() const
    {
        return currentValue_;
    }

    template<typename T>
    void InterpolatingValue<T>::resetTo(T value)
    {
        currentValue_ = value;
        goalValue_ = value;
        remainingTicks_ = 0;
        step_ = 0.f;
    }

    template<typename T>
    T InterpolatingValue<T>::operator++(int)
    {
        if (remainingTicks_ == 0)
            return currentValue_;

        const auto temp = currentValue_;
        currentValue_ = goalValue_ - step_ * static_cast<float>(--remainingTicks_);

        return temp;
    }

    template<typename T>
    T& InterpolatingValue<T>::operator++()
    {
        if (remainingTicks_ == 0)
            return currentValue_;

        currentValue_ = goalValue_ - step_ * static_cast<float>(--remainingTicks_);

        return currentValue_;
    }

    template<typename T>
    void InterpolatingValue<T>::operator=(T value)
    {
        goalValue_ = value;

        if (durationInTicks_ == 0)
        {
            remainingTicks_ = 0;
            step_ = 0.f;
            currentValue_ = value;
            return;
        }
        else if (currentValue_ == goalValue_)
        {
            remainingTicks_ = 0;
            step_ = 0.f;
            return;
        }

        remainingTicks_ = durationInTicks_;
        step_ = (goalValue_ - currentValue_) / static_cast<T>(durationInTicks_);
    }

    template<typename T>
    void InterpolatingValue<T>::operator+=(T value)
    {
        *this = goalValue_ + value;
    }

    template class InterpolatingValue<float>;
    template class InterpolatingValue<double>;
}
