#ifndef _SH_GAME_WORLDPACKET_H_
#define _SH_GAME_WORLDPACKET_H_

#include <common.h>
#include <SHByteBuffer.h>

NAMESPACE_SH_USE

class PacketHeader
{
    public:
        PacketHeader(uint32 pktSize, uint16 msgCode)
        {
            Reset(pktSize, msgCode);
        }

        virtual ~PacketHeader() { }

        inline void Reset(uint32 pktSize, uint16 msgCode)
        {
            m_pktSize = pktSize;
            m_size    = 4;

            m_header[0] = 0xFF & (pktSize >> 8);
            m_header[1] = 0xFF & pktSize;

            m_header[2] = 0xFF & (msgCode >> 8);
            m_header[3] = 0xFF & msgCode;
        }

        inline uint32 size()      const { return m_size;      }
        inline uint32 pktSize()   const { return m_pktSize;   }
        inline uint8 *header()          { return &m_header[0];}

    private:
        uint32 m_size;
        uint32 m_pktSize;

        uint8  m_header[4];
};

class WorldPacket : public SHByteBuffer
{
    public:
        WorldPacket()                                             : SHByteBuffer(0), m_msgCode(0) { }
        WorldPacket(const WorldPacket &worldPacket)               : SHByteBuffer(worldPacket), m_msgCode(worldPacket.m_msgCode) { }
        explicit WorldPacket(uint16 msgCode, size_t size = 200)   : SHByteBuffer(size), m_msgCode(msgCode) { }
        virtual ~WorldPacket() { }

        inline void Initialize(uint16 msgCode, size_t size = 200)
        {
            Clear();
            m_storage.reserve(size);
            m_msgCode = msgCode;
        }

        inline void Reset()
        {
            Clear();
            m_msgCode = 0;
        }

        inline uint16  getMsgCode() const         { return m_msgCode;    }
        inline void    setMsgCode(uint16 msgCode) { m_msgCode = msgCode; }

    protected:
        uint16 m_msgCode;
};

#endif
