
#ifndef ITERATOR_H
#define ITERATOR_H

namespace Common
{
    class Iterator
    {
    private:
        unsigned int m_index;
        unsigned int m_size;
    public:
        Iterator(unsigned int size) : m_index(0), m_size(size) { }
    public:
        void Next() { m_index++; }
        inline unsigned int GetSize() const { return m_size; }
        inline unsigned int GetPosition() const { return m_index; }
        inline bool HasItems() { return m_index < m_size; }
    };
}

#endif // ITERATOR_H

