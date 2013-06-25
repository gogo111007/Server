#ifndef _SH_GAME_CLASS_WORLDSESSION_H_
#define _SH_GAME_CLASS_WORLDSESSION_H_

class WorldSocket;
class WorldPacket;
class Player;

enum ClientType
{
    CLIENT_TYPE_AS               =  0,      // For adobe flash client
    CLIENT_TYPE_CPP              =  1,      // For win32 or other platform based on C++
    CLIENT_TYPE_UNKNOW           = -1,      // Unknow client type.
};

class WorldSession
{
    friend class WorldSocket;

    public:
        WorldSession(WorldSocket *pSocket, uint32 accountID = 0);
        ~WorldSession();

        int32             Update(const uint32 &diff);

        bool              isInQueue()        const   { return m_inQueue;      }
        uint32            getAccountID()     const   { return m_accountID;    }
        ClientType        getClientType()    const   { return m_clientType;   }
        Player           *getPlayer()        const   { return m_pPlayer;      }

        void              setInQueue(bool in)        { m_inQueue = in;        }
        void              setPlayer(Player *pPlayer) { m_pPlayer = pPlayer;   }

        void              KickPlayer();
        void              LogOutPlayer();

        void              HandleNULL(WorldPacket &recvPacket);
        void              HandleEarlyProccess(WorldPacket &recvPacket);
        void              HandleServerSide(WorldPacket &recvPacket);
        void              HandleTest(WorldPacket &recvPacket);

        void              HandleEnumCharacters(WorldPacket &recvPacket);
        void              HandlePlayerInfo(WorldPacket &recvPacket);
        void              HandlePlayerLogin(WorldPacket &recvPacket);
        void              HandlePlayerLogOut(WorldPacket &recvPacket);
        void              HandleChatMessage(WorldPacket &recvPacket);

        void              HandleMoveStartForward(WorldPacket &recvPacket);
        void              HandleMoveStartBackward(WorldPacket &recvPacket);
        void              HandleMoveStop(WorldPacket &recvPacket);
        void              HandleMoveJump(WorldPacket &recvPacket);
        void              HandleMovementInfo(WorldPacket &recvPacket);
        void              HandleMoveStateChanged(WorldPacket &recvPacket);

        int32             SendPacket(WorldPacket const *sendPacket);

    private:
        bool              m_inQueue;
        uint32            m_accountID;

        Player           *m_pPlayer;
        ClientType        m_clientType;
        WorldSocket      *m_pSocket;
};

#endif