#ifndef _SH_GAME_CLASS_WORLDSOCKET_H_
#define _SH_GAME_CLASS_WORLDSOCKET_H_

#include <common.h>
#include <SHFramework.h>

class  WorldPacket;
class  WorldSession;
class  PacketHeader;

NAMESPACE_SH_USE

struct Account
{
    uint32 m_accountID;
    string m_username;
    string m_password;
};

class WorldSocket
{
    friend class SocketEventHandler;

    public:
        WorldSocket(SHSocket *pSHSocket = NULL);
        ~WorldSocket();

        void  Open();
        void  Close();

        void  HandleReceiveData();

        int32 SendPacket(const WorldPacket &sendPacket);

        const string         &getRemoteHost()  const   { return m_pSHSocket->remoteHost(); }
        uint32                getRemotePort()  const   { return m_pSHSocket->remotePort(); }
        const SHSocket       *getSHSocket()    const   { return m_pSHSocket; }
        const WorldSession   *getSession()     const   { return m_pSession;  }
        bool                  closed()         const   { return m_closed;    }

    private:
        void  ProcessIncomingPacket (WorldPacket *recvPacket);
        void  HandleConnection      (WorldPacket &recvPacket);
        void  HandlePing            (WorldPacket &recvPacket);
        void  HandleAuthSession     (WorldPacket &recvPacket);

        bool                    m_closed;
        uint16                  m_missedDataSize;

        SHSocket               *m_pSHSocket;
        WorldSession           *m_pSession;
        WorldPacket            *m_pRecvPacket;
        PacketHeader           *m_pSendHeader;

        vector<byte>            m_recvDataHolder;
        vector<byte>            m_sendDataHolder;
};

#endif