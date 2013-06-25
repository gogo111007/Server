#ifndef _SH_FRAMEWORK_CLASS_SHByteBuffer_H_
#define _SH_FRAMEWORK_CLASS_SHByteBuffer_H_

#include <common.h>
#include <SHByteConverter.h>

NAMESPACE_SH

    NAMESPACE_STD_USE

    #define BITS_1         uint8 _1
    #define BITS_2 BITS_1, uint8 _2
    #define BITS_3 BITS_2, uint8 _3
    #define BITS_4 BITS_3, uint8 _4
    #define BITS_5 BITS_4, uint8 _5
    #define BITS_6 BITS_5, uint8 _6
    #define BITS_7 BITS_6, uint8 _7
    #define BITS_8 BITS_7, uint8 _8

    #define BIT_VALS_1             _1
    #define BIT_VALS_2 BIT_VALS_1, _2
    #define BIT_VALS_3 BIT_VALS_2, _3
    #define BIT_VALS_4 BIT_VALS_3, _4
    #define BIT_VALS_5 BIT_VALS_4, _5
    #define BIT_VALS_6 BIT_VALS_5, _6
    #define BIT_VALS_7 BIT_VALS_6, _7
    #define BIT_VALS_8 BIT_VALS_7, _8

    class ObjectGuid;

    class SHByteBufferException
    {
        public:
            SHByteBufferException(bool _add, size_t _pos, size_t _esize, size_t _size):
                m_add(_add), m_pos(_pos), m_esize(_esize), m_size(_size)
            {
                PrintPosError();
            }

            void PrintPosError() const
            {
                cout << (m_add ? "put " : "get ") << m_pos << ", size: " << m_size << ", esize: " << m_esize << endl;
            }

        private:
            bool   m_add;
            size_t m_pos;
            size_t m_esize;
            size_t m_size;
    };

    class SHBitStream
    {
        public:
            SHBitStream(): m_rpos(0), m_wpos(0) {}

            SHBitStream(uint32 val, size_t len): m_rpos(0), m_wpos(0)
            {
                WriteBits(val, len);
            }

            SHBitStream(SHBitStream const& bs) : m_rpos(bs.m_rpos), m_wpos(bs.m_wpos), m_data(bs.m_data) {}

            void   Clear();
            uint8  GetBit(uint32 bit);
            uint8  ReadBit();
            void   WriteBit(uint32 bit);
            bool   Empty();
            void   Reverse();
            void   Print();

            template <typename T> void WriteBits(T value, size_t bits);

            size_t     GetLength()                   { return m_data.size();   }
            uint32     GetReadPosition()             { return m_rpos;          }
            uint32     GetWritePosition()            { return m_wpos;          }
            void       SetReadPos(uint32 pos)        { m_rpos = pos;           }

            uint8 const &operator[](uint32 const pos) const
            {
                return m_data[pos];
            }

            uint8 &operator[] (uint32 const pos)
            {
                return m_data[pos];
            }

        private:
            std::vector<uint8> m_data;
            uint32 m_rpos, m_wpos;
    };

    template<class T>
    struct Unused
    {
        Unused() {}
    };

    class SHByteBuffer
    {
        public:
            const static size_t DEFAULT_SIZE = 64;

            // constructor
            SHByteBuffer(): m_rpos(0), m_wpos(0), m_bitpos(8), m_curbitval(0)
            {
                m_storage.reserve(DEFAULT_SIZE);
            }

            // constructor
            SHByteBuffer(size_t size): m_rpos(0), m_wpos(0), m_bitpos(8), m_curbitval(0)
            {
                m_storage.reserve(size);
            }

            // copy constructor
            SHByteBuffer(const SHByteBuffer &buf) : m_rpos(buf.m_rpos), m_wpos(buf.m_wpos),
                m_storage(buf.m_storage), m_bitpos(buf.m_bitpos), m_curbitval(buf.m_curbitval)
            {
            }

            virtual ~SHByteBuffer() { }

            void Clear()
            {
                m_storage.clear();
                m_rpos       = 0;
                m_wpos       = 0;
                m_curbitval  = 0;
                m_bitpos     = 8;
            }

            template <typename T> SHByteBuffer &Append(T value)
            {
                FlushBits();
                EndianConvert(value);
                return Append((uint8*)&value, sizeof(value));
            }

            void FlushBits()
            {
                if (m_bitpos == 8)
                    return;

                Append((uint8 *)&m_curbitval, sizeof(uint8));
                m_curbitval = 0;
                m_bitpos = 8;
            }

            void ResetBitReader()
            {
                m_bitpos = 8;
            }

            template <typename T> bool WriteBit(T bit)
            {
                --m_bitpos;
                if (bit)
                    m_curbitval |= (1 << (m_bitpos));

                if (m_bitpos == 0)
                {
                    m_bitpos = 8;
                    Append((uint8 *)&m_curbitval, sizeof(m_curbitval));
                    m_curbitval = 0;
                }

                return (bit != 0);
            }

            bool ReadBit()
            {
                ++m_bitpos;
                if (m_bitpos > 7)
                {
                    m_curbitval = Read<uint8>();
                    m_bitpos = 0;
                }

                return ((m_curbitval >> (7 - m_bitpos)) & 1) != 0;
            }

            template <typename T> void WriteBits(T value, size_t bits)
            {
                for (int32 i = bits - 1; i >= 0; --i)
                    WriteBit((value >> i) & 1);
            }

            uint32 ReadBits(size_t bits)
            {
                uint32 value = 0;
                for (int32 i = bits-1; i >= 0; --i)
                    if (ReadBit())
                        value |= (1 << i);

                return value;
            }

            SHBitStream ReadSHBitStream(uint32 len)
            {
                SHBitStream b;
                for (uint32 i = 0; i < len; ++i)
                    b.WriteBit(ReadBit());
                return b;
            }

            void WriteGuidMask(uint64 guid, uint8 *maskOrder, uint8 maskCount, uint8 maskPos = 0)
            {
                uint8* guidByte = ((uint8*)&guid);

                for (uint8 i = 0; i < maskCount; i++)
                    WriteBit(guidByte[maskOrder[i + maskPos]]);
            }

            void WriteGuidBytes(uint64 guid, uint8 *byteOrder, uint8 byteCount, uint8 bytePos)
            {
                uint8* guidByte = ((uint8*)&guid);

                for (uint8 i = 0; i < byteCount; i++)
                    if (guidByte[byteOrder[i + bytePos]])
                        (*this) << uint8(guidByte[byteOrder[i + bytePos]] ^ 1);
            }

            template<BITS_1>
                void ReadGuidMask(ObjectGuid &guid);
            template<BITS_2>
                void ReadGuidMask(ObjectGuid &guid);
            template<BITS_3>
                void ReadGuidMask(ObjectGuid &guid);
            template<BITS_4>
                void ReadGuidMask(ObjectGuid &guid);
            template<BITS_5>
                void ReadGuidMask(ObjectGuid &guid);
            template<BITS_6>
                void ReadGuidMask(ObjectGuid &guid);
            template<BITS_7>
                void ReadGuidMask(ObjectGuid &guid);
            template<BITS_8>
                void ReadGuidMask(ObjectGuid &guid);

            template<BITS_1>
                void WriteGuidMask(ObjectGuid guid);
            template<BITS_2>
                void WriteGuidMask(ObjectGuid guid);
            template<BITS_3>
                void WriteGuidMask(ObjectGuid guid);
            template<BITS_4>
                void WriteGuidMask(ObjectGuid guid);
            template<BITS_5>
                void WriteGuidMask(ObjectGuid guid);
            template<BITS_6>
                void WriteGuidMask(ObjectGuid guid);
            template<BITS_7>
                void WriteGuidMask(ObjectGuid guid);
            template<BITS_8>
                void WriteGuidMask(ObjectGuid guid);

            template<BITS_1>
                void ReadGuidBytes(ObjectGuid &guid);
            template<BITS_2>
                void ReadGuidBytes(ObjectGuid &guid);
            template<BITS_3>
                void ReadGuidBytes(ObjectGuid &guid);
            template<BITS_4>
                void ReadGuidBytes(ObjectGuid &guid);
            template<BITS_5>
                void ReadGuidBytes(ObjectGuid &guid);
            template<BITS_6>
                void ReadGuidBytes(ObjectGuid &guid);
            template<BITS_7>
                void ReadGuidBytes(ObjectGuid &guid);
            template<BITS_8>
                void ReadGuidBytes(ObjectGuid &guid);

            template<BITS_1>
                void WriteGuidBytes(ObjectGuid guid);
            template<BITS_2>
                void WriteGuidBytes(ObjectGuid guid);
            template<BITS_3>
                void WriteGuidBytes(ObjectGuid guid);
            template<BITS_4>
                void WriteGuidBytes(ObjectGuid guid);
            template<BITS_5>
                void WriteGuidBytes(ObjectGuid guid);
            template<BITS_6>
                void WriteGuidBytes(ObjectGuid guid);
            template<BITS_7>
                void WriteGuidBytes(ObjectGuid guid);
            template<BITS_8>
                void WriteGuidBytes(ObjectGuid guid);

            template <typename T> void Put(size_t pos, T value)
            {
                EndianConvert(value);
                Put(pos, (uint8 *)&value, sizeof(value));
            }

            SHByteBuffer &operator<<(uint8 value)
            {
                Append<uint8>(value);
                return *this;
            }

            SHByteBuffer &operator<<(uint16 value)
            {
                Append<uint16>(value);
                return *this;
            }

            SHByteBuffer &operator<<(uint32 value)
            {
                Append<uint32>(value);
                return *this;
            }

            SHByteBuffer &operator<<(uint64 value)
            {
                Append<uint64>(value);
                return *this;
            }

            // signed as in 2e complement
            SHByteBuffer &operator<<(int8 value)
            {
                Append<int8>(value);
                return *this;
            }

            SHByteBuffer &operator<<(int16 value)
            {
                Append<int16>(value);
                return *this;
            }

            SHByteBuffer &operator<<(int32 value)
            {
                Append<int32>(value);
                return *this;
            }

            SHByteBuffer &operator<<(int64 value)
            {
                Append<int64>(value);
                return *this;
            }

            // floating points
            SHByteBuffer &operator<<(float value)
            {
                Append<float>(value);
                return *this;
            }

            SHByteBuffer &operator<<(double value)
            {
                Append<double>(value);
                return *this;
            }

            SHByteBuffer &operator<<(bool value)
            {
                Append<bool>(value);
                return *this;
            }

            SHByteBuffer &operator<<(const std::string &value)
            {
                Append((uint8)0);
                Append((uint8 const *)value.c_str(), value.length());
                Append((uint8)0);
                return *this;
            }

            SHByteBuffer &operator<<(const char *str)
            {
                Append((uint8)0);
                Append((uint8 const *)str, str ? strlen(str) : 0);
                Append((uint8)0);
                return *this;
            }

            SHByteBuffer &operator>>(bool &value)
            {
                value = Read<char>() > 0 ? true : false;
                return *this;
            }

            SHByteBuffer &operator>>(uint8 &value)
            {
                value = Read<uint8>();
                return *this;
            }

            SHByteBuffer &operator>>(uint16 &value)
            {
                value = Read<uint16>();
                return *this;
            }

            SHByteBuffer &operator>>(uint32 &value)
            {
                value = Read<uint32>();
                return *this;
            }

            SHByteBuffer &operator>>(uint64 &value)
            {
                value = Read<uint64>();
                return *this;
            }

            //signed as in 2e complement
            SHByteBuffer &operator>>(int8 &value)
            {
                value = Read<int8>();
                return *this;
            }

            SHByteBuffer &operator>>(int16 &value)
            {
                value = Read<int16>();
                return *this;
            }

            SHByteBuffer &operator>>(int32 &value)
            {
                value = Read<int32>();
                return *this;
            }

            SHByteBuffer &operator>>(int64 &value)
            {
                value = Read<int64>();
                return *this;
            }

            SHByteBuffer &operator>>(float &value)
            {
                value = Read<float>();
                return *this;
            }

            SHByteBuffer &operator>>(double &value)
            {
                value = Read<double>();
                return *this;
            }

            SHByteBuffer &operator>>(std::string& value)
            {
                value.clear();

                ReadSkip<uint8>();                              // for as3 client we need to add a '0' at the start of the string
                while (Rpos() < Size())                         // prevent crash at wrong string format in packet
                {
                    char c = Read<char>();
                    if (c == 0)
                        break;
                    value += c;
                }
                return *this;
            }

            template<class T>
            SHByteBuffer &operator>>(Unused<T> const&)
            {
                return ReadSkip<T>();
            }

            uint8 &operator[](size_t const pos)
            {
                if (pos >= Size())
                    throw SHByteBufferException(false, pos, 1, Size());
                return m_storage[pos];
            }

            uint8 const &operator[](size_t const pos) const
            {
                if (pos >= Size())
                    throw SHByteBufferException(false, pos, 1, Size());
                return m_storage[pos];
            }

            size_t Rpos() const { return m_rpos; }

            size_t Rpos(size_t rpos_)
            {
                m_rpos = rpos_;
                return m_rpos;
            }

            void RFinish()
            {
                m_rpos = Wpos();
            }

            size_t Wpos() const { return m_wpos; }

            size_t Wpos(size_t wpos)
            {
                m_wpos = wpos;
                return m_wpos;
            }

            template<typename T>
            SHByteBuffer &ReadSkip()
            {
                ReadSkip(sizeof(T));
                return *this;
            }

            SHByteBuffer &ReadSkip(size_t skip)
            {
                ResetBitReader();
                if(m_rpos + skip > Size())
                    throw SHByteBufferException(false, m_rpos, skip, Size());
                m_rpos += skip;

                return *this;
            }

            template <typename T> T Read()
            {
                ResetBitReader();
                T r = Read<T>(m_rpos);
                m_rpos += sizeof(T);
                return r;
            }

            template <typename T> T Read(size_t pos) const
            {
                if(pos + sizeof(T) > Size())
                    throw SHByteBufferException(false, pos, sizeof(T), Size());
                T val = *((T const*)&m_storage[pos]);
                EndianConvert(val);
                return val;
            }

            SHByteBuffer &Read(uint8* dest, size_t len)
            {
                ResetBitReader();
                if(m_rpos  + len > Size())
                    throw SHByteBufferException(false, m_rpos, len, Size());
                memcpy(dest, &m_storage[m_rpos], len);
                m_rpos += len;

                return *this;
            }

            uint64 ReadPackGUID()
            {
                uint64 guid = 0;
                uint8 guidmark = 0;
                (*this) >> guidmark;

                for(int32 i = 0; i < 8; ++i)
                {
                    if(guidmark & (uint8(1) << i))
                    {
                        uint8 bit;
                        (*this) >> bit;
                        guid |= (uint64(bit) << (i * 8));
                    }
                }

                return guid;
            }

            uint8 ReadUInt8()
            {
                uint8 u = 0;
                (*this) >> u;
                return u;
            }

            uint16 ReadUInt16()
            {
                uint16 u = 0;
                (*this) >> u;
                return u;
            }

            uint32 ReadUInt32()
            {
                uint32 u = 0;
                (*this) >> u;
                return u;
            }

            uint64 ReadUInt64()
            {
                uint64 u = 0;
                (*this) >> u;
                return u;
            }

            int8 ReadInt8()
            {
                int8 u = 0;
                (*this) >> u;
                return u;
            }

            int16 ReadInt16()
            {
                int16 u = 0;
                (*this) >> u;
                return u;
            }

            int32 ReadInt32()
            {
                uint32 u = 0;
                (*this) >> u;
                return u;
            }

            int64 ReadInt64()
            {
                int64 u = 0;
                (*this) >> u;
                return u;
            }

            std::string ReadString()
            {
                std::string s = "";
                (*this) >> s;
                return s;
            }

            std::string ReadString(uint32 count)
            {
                std::string out;
                ReadSkip<uint8>();
                uint32 start = Rpos();
                while (Rpos() < Size() && Rpos() < start + count)       // prevent crash at wrong string format in packet
                    out += Read<char>();

                return out;
            }

            SHByteBuffer &WriteStringData(const std::string &str)
            {
                FlushBits();
                return Append((uint8 const*)str.c_str(), str.size());
            }

            bool ReadBoolean()
            {
                uint8 b = 0;
                (*this) >> b;
                return b > 0 ? true : false;
            }

            float ReadSingle()
            {
                float f = 0;
                (*this) >> f;
                return f;
            }

            const uint8 *Contents() const { return &m_storage[0]; }
            size_t Size()           const { return m_storage.size();  }
            bool   Empty()          const { return m_storage.empty(); }

            void Resize(size_t newsize)
            {
                m_storage.resize(newsize);
                m_rpos = 0;
                m_wpos = Size();
            }

            void Reserve(size_t ressize)
            {
                if (ressize > Size())
                    m_storage.reserve(ressize);
            }

            SHByteBuffer &Append(const std::string &str)
            {
                Append((uint8)0);
                return Append((uint8 const*)str.c_str(), str.size() + 1);
            }

            SHByteBuffer &Append(const char *pSrc, size_t cnt)
            {
                Append((uint8)0);
                return Append((const uint8 *)pSrc, cnt);
            }

            template<class T> SHByteBuffer &Append(const T *pSrc, size_t cnt)
            {
                return Append((const uint8 *)pSrc, cnt * sizeof(T));
            }

            SHByteBuffer &Append(const uint8 *pSrc, size_t cnt)
            {
                if (!cnt)
                    return *this;

                if (Size() >= 10000000)
                    return *this;

                if (m_storage.size() < m_wpos + cnt)
                    m_storage.resize(m_wpos + cnt);
                memcpy(&m_storage[m_wpos], pSrc, cnt);
                m_wpos += cnt;

                return *this;
            }

            SHByteBuffer &Append(const SHByteBuffer &buffer)
            {
                if (buffer.Wpos())
                    return Append(buffer.Contents(), buffer.Wpos());

                return *this;
            }

            SHByteBuffer &AppendPackXYZ(float x, float y, float z)
            {
                uint32 packed = 0;
                packed |= ((int32)(x / 0.25f) & 0x7FF);
                packed |= ((int32)(y / 0.25f) & 0x7FF) << 11;
                packed |= ((int32)(z / 0.25f) & 0x3FF) << 22;
                *this << packed;

                return *this;
            }

            SHByteBuffer &AppendPackGUID(uint64 guid)
            {
                uint8 packGUID[8+1];
                packGUID[0] = 0;
                size_t size = 1;
                for (uint8 i = 0; guid != 0; ++i)
                {
                    if (guid & 0xFF)
                    {
                        packGUID[0] |= uint8(1 << i);
                        packGUID[size] =  uint8(guid & 0xFF);
                        ++size;
                    }

                    guid >>= 8;
                }
                return Append(packGUID, size);
            }

            void Put(size_t pos, const uint8 *pSrc, size_t cnt)
            {
                if(pos + cnt > Size())
                    throw SHByteBufferException(true, pos, cnt, Size());
                memcpy(&m_storage[pos], pSrc, cnt);
            }

            void PrintStorage() const
            {
                cout << "STORAGE_SIZE: " << (unsigned long)Size() << ": ";
                for (uint32 i = 0; i < Size(); ++i)
                    cout << Read<uint8>(i) << " - ";
                cout << " \n";
            }

            void Textlike() const
            {
                cout << "STORAGE_SIZE: " << (unsigned long)Size() << ": ";
                for (uint32 i = 0; i < Size(); ++i)
                    cout << Read<uint8>(i);
                cout << " \n";
            }

            void Hexlike() const
            {
                uint32 j = 1, k = 1;
                cout << "STORAGE_SIZE: " << (unsigned long)Size() << ": ";

                for (uint32 i = 0; i < Size(); ++i)
                {
                    if ((i == (j * 8)) && ((i != (k * 16))))
                    {
                        if (Read<uint8>(i) < 0x10)
                        {
                            cout << "|0x" << Read<uint8>(i);
                        }
                        else
                        {
                            cout << "|x" << Read<uint8>(i);
                        }
                        ++j;
                    }
                    else if (i == (k * 16))
                    {
                        if (Read<uint8>(i) < 0x10)
                        {
                            cout << "\n";

                            cout << "0x" << Read<uint8>(i);
                        }
                        else
                        {
                            cout << "\n";

                            cout << "x " << Read<uint8>(i);
                        }

                        ++k;
                        ++j;
                    }
                    else
                    {
                        if (Read<uint8>(i) < 0x10)
                        {
                            cout << "0x" << Read<uint8>(i);
                        }
                        else
                        {
                            cout << "x " << Read<uint8>(i);
                        }
                    }
                }
                cout << "\n";
            }

        protected:
            size_t m_rpos, m_wpos, m_bitpos;
            uint8 m_curbitval;
            std::vector<uint8> m_storage;
    };

    class BitConverter
    {
        public:
            static uint8 ToUInt8(SHByteBuffer const &buff, size_t start = 0)
            {
                return buff.Read<uint8>(start);
            }

            static uint16 ToUInt16(SHByteBuffer const &buff, size_t start = 0)
            {
                return buff.Read<uint16>(start);
            }

            static uint32 ToUInt32(SHByteBuffer const &buff, size_t start = 0)
            {
                return buff.Read<uint32>(start);
            }

            static uint64 ToUInt64(SHByteBuffer const &buff, size_t start = 0)
            {
                return buff.Read<uint64>(start);
            }

            static int16 ToInt16(SHByteBuffer const &buff, size_t start = 0)
            {
                return buff.Read<int16>(start);
            }

            static int32 ToInt32(SHByteBuffer const &buff, size_t start = 0)
            {
                return buff.Read<int32>(start);
            }

            static int64 ToInt64(SHByteBuffer const &buff, size_t start = 0)
            {
                return buff.Read<int64>(start);
            }
    };

NAMESPACE_SH_END

#endif
