#pragma once

#include <atomic>

template<class T>
class CopyableAtomic : public std::atomic<T>
{
public:
    using std::atomic<T>::atomic;

    constexpr CopyableAtomic(const CopyableAtomic<T>& other)
        : CopyableAtomic(other.load())
    {
    }

    CopyableAtomic& operator=(const CopyableAtomic<T>& other)
    {
        store(other.load());
        return *this;
    }
};