#include <SHByteBuffer.h>

template <typename T>
inline SHByteBuffer &operator<<(SHByteBuffer &b, std::vector<T> v)
{
    b << (uint32)v.size();
    for (typename std::vector<T>::iterator i = v.begin(); i != v.end(); ++i)
    {
        b << *i;
    }
    return b;
}

template <typename T>
inline SHByteBuffer &operator>>(SHByteBuffer &b, std::vector<T> &v)
{
    uint32 vsize;
    b >> vsize;
    v.Clear();
    while (vsize--)
    {
        T t;
        b >> t;
        v.push_back(t);
    }
    return b;
}

template <typename T>
inline SHByteBuffer &operator<<(SHByteBuffer &b, std::list<T> v)
{
    b << (uint32)v.size();
    for (typename std::list<T>::iterator i = v.begin(); i != v.end(); ++i)
    {
        b << *i;
    }
    return b;
}

template <typename T>
inline SHByteBuffer &operator>>(SHByteBuffer &b, std::list<T> &v)
{
    uint32 vsize;
    b >> vsize;
    v.Clear();
    while (vsize--)
    {
        T t;
        b >> t;
        v.push_back(t);
    }
    return b;
}

template <typename K, typename V>
inline SHByteBuffer &operator<<(SHByteBuffer &b, std::map<K, V> &m)
{
    b << (uint32)m.size();
    for (typename std::map<K, V>::iterator i = m.begin(); i != m.end(); ++i)
    {
        b << i->first << i->second;
    }
    return b;
}

template <typename K, typename V>
inline SHByteBuffer &operator>>(SHByteBuffer &b, std::map<K, V> &m)
{
    uint32 msize;
    b >> msize;
    m.Clear();
    while (msize--)
    {
        K k;
        V v;
        b >> k >> v;
        m.insert(make_pair(k, v));
    }
    return b;
}

// TODO: Make a SHByteBuffer.cpp and move all this inlining to it.
template<> inline std::string SHByteBuffer::read<std::string>()
{
    std::string tmp;
    *this >> tmp;
    return tmp;
}

template<>
inline SHByteBuffer& SHByteBuffer::read_skip<char*>()
{
    std::string temp;
    *this >> temp;

    return *this;
}

template<>
inline SHByteBuffer& SHByteBuffer::read_skip<char const*>()
{
    return read_skip<char*>();
}

template<>
inline SHByteBuffer& SHByteBuffer::read_skip<std::string>()
{
    return read_skip<char*>();
}