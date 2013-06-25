#include "WorldSocket.h"
#include "SocketEventHandler.h"

void SocketEventHandler::OnAccept(SHSocket *pSHSocket, SHSocketGroup *pGroup, void *pReference)
{
    auto *pNewSocket   = pSHSocket->Accept();
    auto *pWorldSocket = new WorldSocket(pNewSocket);

    m_worldSocketMap[pNewSocket] = pWorldSocket;
    pGroup->Add(pNewSocket);

    pWorldSocket->Open();
}

void SocketEventHandler::OnRead(SHSocket *pSHSocket, SHSocketGroup *pGroup, void *pReference)
{
    SAFE_CALL_VOID(m_worldSocketMap[pSHSocket], HandleReceiveData());
}

void SocketEventHandler::OnDisconnect(SHSocket *pSHSocket, SHSocketGroup *pGroup, void *pReference)
{
    cout << "Client " << pSHSocket->remoteHost() << " disconnected...\n";

    SAFE_DELETE(m_worldSocketMap[pSHSocket]);

    m_worldSocketMap.erase(pSHSocket);
    pGroup->Remove(pSHSocket);

    SAFE_DELETE(pSHSocket);
}
