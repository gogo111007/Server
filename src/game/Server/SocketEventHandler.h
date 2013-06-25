#ifndef _SH_GAME_SOCKETEVENTHANDLER_H_
#define _SH_GAME_SOCKETEVENTHANDLER_H_

#include <common.h>
#include <SHSocket.h>
#include <SHSocketGroup.h>

class WorldSocket;

NAMESPACE_SH_USE
NAMESPACE_STD_USE

class SocketEventHandler: public SHSocketEventHandler
{
    public:
        SocketEventHandler() { }
        virtual ~SocketEventHandler() { }

        void OnAccept              (SHSocket *pSocket, SHSocketGroup *pGroup, void *pReference);
        void OnRead                (SHSocket *pSocket, SHSocketGroup *pGroup, void *pReference);
        void OnDisconnect          (SHSocket *pSocket, SHSocketGroup *pGroup, void *pReference);

    private:
        unordered_map<SHSocket*, WorldSocket*> m_worldSocketMap;
};

#endif;