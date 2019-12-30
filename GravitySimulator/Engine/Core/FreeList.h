#pragma once

#include <vector>

// Indexed free list with constant-time removals from anywhere without invalidating indices
template<class T>
class FreeList
{
    using IndexType = int32_t;

    // Can't be a union since T can be a non-trivial type
    struct FreeElement
    {
        T element;
        IndexType next = -1;
    };

public:
    FreeList() = default;

    void reserve(size_t capacity)
    {
        m_data.reserve(capacity);
    }

    void resize(size_t size)
    {
        m_data.resize(size);
    }

    size_t capacity() const noexcept
    {
        return m_data.capacity();
    }

    size_t size() const noexcept
    {
        return m_data.size();
    }

    // Inserts an element to the free list and returns an index to it
    IndexType insert(const T& element)
    {
        if (m_firstFree != -1)
        {
            const auto index = m_firstFree;
            m_firstFree = m_data[m_firstFree].next;
            m_data[index].element = element;
            return index;
        }
        else
        {
            m_data.push_back({ element });
            return static_cast<IndexType>(m_data.size() - 1);
        }
    }

    // Removes the nth element from the free list
    void erase(IndexType n)
    {
        m_data[n].next = m_firstFree;
        m_firstFree = n;
    }

    // Removes all elements from the free list
    void clear() noexcept
    {
        m_data.clear();
        m_firstFree = -1;
    }

    // Returns the nth element
    T& operator[](IndexType n)
    {
        return m_data[n].element;
    }

    // Returns the nth element
    const T& operator[](IndexType n) const
    {
        return m_data[n].element;
    }

private:
    std::vector<FreeElement> m_data;
    IndexType m_firstFree = -1;
};