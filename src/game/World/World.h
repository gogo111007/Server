#ifndef _SH_GAME_CLASS_WORLD_H_
#define _SH_GAME_CLASS_WORLD_H_

#include <common.h>

#include "MsgCode.h"

class WorldSocket;
class WorldPacket;
class WorldSession;
class WorldTimer;

NAMESPACE_STD_USE

class World
{
    typedef UNORDERED_MAP<uint32, WorldSession*> SessionMap;
    typedef UNORDERED_MAP<uint32, WorldSession*>::const_iterator SessionMapItr;

    public:
        static volatile uint32 m_worldLoopCount;

        World() { }
        ~World();

        void           Init();
        void           Release();

        void           Update(uint32 diff);
        void           UpdateSessions(uint32 diff);

        void           AddSession(WorldSession *pSession);
        void           AddQueuedSession(WorldSession *pSession);
        bool           RemoveSession(WorldSession *pSession);
        bool           RemoveQueuedSession(WorldSession *pSession);
        WorldSession  *FindSession(uint32 id) const;
        inline uint32  GetTotalSessionCount()   const { return m_sessionMap.size(); }
        inline uint32  GetActivedSessionCount() const { return m_sessionMap.size() - m_queuedSessionQueue.size(); }
        inline uint32  GetQueuedSessionCount()  const { return m_queuedSessionQueue.size(); }
        inline bool    stopped()                const { return m_stopped; }

        void           ShutDown(uint32 delay = 0);
        bool           IsShuttingDown();
        bool           CancleShutDown();

        bool           KickPlayer(uint32 id);
        void           KickAllPlayer();

        void           SendWorldMessage(const string msg, int32 sender = -1);
        void           SendGlobalWorldPacket(WorldPacket &data);

    private:
        static volatile bool m_stopped;

        void           _AddSession(WorldSession *pSession);

        SessionMap            m_sessionMap;
        deque<WorldSession*>  m_newSessionQueue;
        deque<WorldSession*>  m_queuedSessionQueue;
};

#define sWorld SHSingleton<World>::instance()

#endif