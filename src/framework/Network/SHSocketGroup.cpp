#include <SHSocketGroup.h>

NAMESPACE_SH_USE

void SHSocketGroup::Remove(SHSocket *pSocket)
{
    vector<SHSocket*>::iterator it = m_socketList.begin();

    while (it != m_socketList.end())
    {
        if (*it == pSocket)
        {
            m_socketList.erase(it);
            return;
        }
        else
            ++it;
    }
}

bool SHSocketGroup::Listen(uint32 milisec, void *pReference)
{
    uint32 finTime = getTime() + milisec;
    bool HandleSocketEventutedOnce = false;
    bool result = false;

    while (getTime() < finTime || !HandleSocketEventutedOnce)
    {
        HandleSocketEventutedOnce = true;

        fd_set setSockets;
        int32 maxHandle = 0;

        FD_ZERO(&setSockets);

        for (uint32 i = 0; i < m_socketList.size(); i++)
        {
            FD_SET(m_socketList[i]->socketHandler(), &setSockets);
            maxHandle = iMax(maxHandle, m_socketList[i]->socketHandler());
        }

        uint32 now = getTime();
        uint32 milisecLeft = finTime > now ? finTime - now : 0;
        struct timeval timeout;

        timeout.tv_sec  = milisecLeft / 1000;
        timeout.tv_usec = (milisecLeft % 1000) * 1000;

        int32 status = select(maxHandle + 1, &setSockets, NULL, NULL, &timeout);

        if (status == -1)
            throw SHException(SHException::SH_EXCEPTION_NETWORK_SELECT, "SHSocketGroup::listen: could not perform socket select");

        uint32 i = 0;
        int32 launchSockets = 0;

        while (launchSockets < status && i < m_socketList.size())
        {
            if (FD_ISSET(m_socketList[i]->socketHandler(), &setSockets) && m_pSocketEventHandler)
            {
                launchSockets++;

                if (m_socketList[i]->type() == SERVER && m_socketList[i]->protocol() == TCP)
                    m_pSocketEventHandler->OnAccept(m_socketList[i], this, pReference);
                else if (m_socketList[i]->protocol() == TCP && !m_socketList[i]->nextReadSize())
                    m_pSocketEventHandler->OnDisconnect(m_socketList[i], this, pReference);
                else
                    m_pSocketEventHandler->OnRead(m_socketList[i], this, pReference);
            }

            ++i;
        }

        if (launchSockets)
            result = true;
    }

    return result;
}
