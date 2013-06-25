#include <SHByteBuffer.h>

NAMESPACE_SH

    void SHBitStream::Clear()
    {
        m_data.clear();
        m_rpos = m_wpos = 0;
    }

    uint8 SHBitStream::GetBit(uint32 bit)
    {
        if (m_data.size() < bit)
            return 0;

        return m_data[bit];
    }

    uint8 SHBitStream::ReadBit()
    {
        if (m_data.size() < m_rpos)
            return 0;

        uint8 b = m_data[m_rpos];
        ++m_rpos;
        return b;
    }

    void SHBitStream::WriteBit(uint32 bit)
    {
        m_data.push_back(bit ? uint8(1) : uint8(0));
        ++m_wpos;
    }

    template <typename T> void SHBitStream::WriteBits(T value, size_t bits)
    {
        for (int32 i = bits-1; i >= 0; --i)
            WriteBit((value >> i) & 1);
    }

    bool SHBitStream::Empty()
    {
        return m_data.empty();
    }

    void SHBitStream::Reverse()
    {
        uint32 len = GetLength();
        std::vector<uint8> b = m_data;
        Clear();

        for(uint32 i = len; i > 0; --i)
            WriteBit(b[i-1]);
    }

    void SHBitStream::Print()
    {
        std::stringstream ss;
        ss << "SHBitStream: ";
        for (uint32 i = 0; i < GetLength(); ++i)
            ss << uint32(GetBit(i)) << " ";

        cout << ss.str().c_str();
    }

NAMESPACE_SH_END
