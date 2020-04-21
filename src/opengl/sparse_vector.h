#ifndef SPARSE_VECTOR_H
#define SPARSE_VECTOR_H

#include <vector>
#include <queue>

template <typename T>
class sparse_vector {
public:
    sparse_vector() : m_size(0) { }

    size_t size() const { return m_size; }
    size_t realSize() const { return m_data.size(); }

    size_t insert(const T &item)
    {
        size_t itemIndex;

        if(!m_free.empty()) {
            itemIndex = m_free.front();
            m_free.pop();
            m_data[itemIndex] = item;
        } else {
            itemIndex = m_data.size();
            m_data.push_back(item);
        }

        m_size++;

        return itemIndex;
    }

    void remove(size_t index)
    {
        if(index < m_data.size() - 1) {
            m_free.push(index);
            m_data[index] = T();
        } else {
            m_data.pop_back();
        }

        m_size--;
    }

    std::vector<T> &data() { return m_data; }
    void clear()
    {
        m_data.clear();
        m_size = 0;

        while(m_free.size())
            m_free.pop();
    }

    T& operator[](size_t index) { return m_data[index]; }

private:
    std::vector<T> m_data;
    std::queue<size_t> m_free;
    size_t m_size;
};

#endif
