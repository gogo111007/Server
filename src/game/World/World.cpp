#include "World.h"
#include "TimeLine.h"
#include "WorldSocket.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "MapManager.h"
#include "ObjectManager.h"
#include "Player.h"

volatile uint32 World::m_worldLoopCount = 0;
volatile bool   World::m_stopped        = false;

World::~World()
{
}

void World::Init()
{
    auto t = new WorldTimer(150, 10,
    [] (void *pTimer)                         // tick function
    {
        cout << "Timer test ticked!! count: " << ((WorldTimer*)pTimer)->getCurrentCount() << endl;
    }, "test",                                // timer name
    [] (void *pTimer)                         // complete function
    {
        cout << "Timer test complete!! Total elapsed time: " << ((WorldTimer*)pTimer)->getElapsedTime() / 1000.0f << " seconds" << endl;

        auto t = new WorldTimer(1000, 0,     // create a new timer when timer test complete
        [] (void *pTimer)                    // tick function
        {
            cout << "Timer test1 ticked!! count: " << ((WorldTimer*)pTimer)->getCurrentCount() << endl;

            if (((WorldTimer*)pTimer)->getCurrentCount() == 10)
            {
                ((WorldTimer*)pTimer)->Pause();             // after 10 ticks, pause timer test1

                cout << "Timer test1 paused!! \nTotal elapsed time: " << ((WorldTimer*)pTimer)->getElapsedTime() / 1000.0f << " seconds" << endl;

                auto tn1 = new WorldTimer(300, 20,          // create a new timer
                [] (void *pTimer)
                {
                    cout << "Timer timer2 ticked!! count: " << ((WorldTimer*)pTimer)->getCurrentCount() << ", elapsed Time: " << ((WorldTimer*)pTimer)->getElapsedTime() / 1000.0f << " seconds" <<endl;
                }, "",
                [&, pTimer] (void *)                       // when this timer complete, start the paused timer test1.
                {
                    cout << "Timer timer2 complete!! Now start the paused timer test1..." << endl;

                    ((WorldTimer*)pTimer)->Start();
                });
                tn1->Start();
            }

            if (((WorldTimer*)pTimer)->getCurrentCount() == 30)   // restart timer test1 after 30 ticks!
            {
                cout << "Timer test1 ticked for 30 times, now it will restart! \nTotal elapsed time: " << ((WorldTimer*)pTimer)->getElapsedTime() / 1000.0f << " seconds" << endl;
                ((WorldTimer*)pTimer)->Restart();
            }
        }, "test1");
        t->Start();
    });

    //t->Start();

    new WorldTimer(3000, 0, nullptr, "interval", nullptr, nullptr, false);   // an interval timer without tickhandler can be more useful sometimes.
}

void World::Update(uint32 diff)
{
    auto t = sTimeLine.FindTimer("interval");

    if (t && t->passed(diff))              // update the interval timer when world update or in any update function.
    {
        cout << "Timer interval ticked!! Current tick count: " << t->getCurrentCount() << endl;

        if (t->getCurrentCount() == 10)
        {
            cout << "Timer interval ticked 10 times!! Now remove all timers and shut down server.\nTotal elapsed time: " << t->getElapsedTime() / 1000.0f << " seconds" << endl;
            sTimeLine.RemoveAllTimers();   // the interval timer ticked for 10 times, now remove all timers.
            ShutDown(3000000);              // Shut down server in 5 minutes...
        }
    }

    UpdateSessions(diff);

    sMapManager.Update(diff);
}

void World::UpdateSessions(uint32 diff)
{
    for (auto s : m_newSessionQueue)
        _AddSession(s);

    m_newSessionQueue.clear();

    auto itr = m_sessionMap.begin();

    while (itr != m_sessionMap.end())
    {
        if (itr->second->Update(diff) == -1)
        {
            delete itr->second;
            itr = m_sessionMap.erase(itr);
        }
        else
            ++itr;
    }
}

void World::Release()
{
    if (!m_stopped)
        ShutDown();

    KickAllPlayer();
    UpdateSessions(1);
}

void World::ShutDown(uint32 delay)
{
    if (delay == 0)
    {
        m_stopped = true;
        cout << "Shutting down server..." << endl;
    }
    else
    {
        if (delay < 1000)
            delay = 1000;

        new WorldTimer(1000, delay / 1000,
        [&, this] (void *pTimer)
        {
            auto t            = (WorldTimer*)pTimer;
            auto remaindCount = t->getRepeatCount() - t->getCurrentCount();

            if ((remaindCount <= 5  * 60 && (t->getCurrentCount() % 15 ) == 0) ||
                (remaindCount <= 15 * 60 && (t->getCurrentCount() % 60 ) == 0) ||
                (remaindCount <= 30 * 60 && (t->getCurrentCount() % 300) == 0) ||
                (remaindCount <= 60 * 60 && (t->getCurrentCount() % 600) == 0))
            {
                cout << "Server will shutdown in " << remaindCount << " seconds !!" << endl;
            }

        }, "shutdownTimer",
        [&, this] (void *)
        {
            ShutDown();
        }, nullptr, true);

        cout << "Server will shutdown in " << delay / 1000.0f << " seconds!!" << endl;
    }
}

bool World::IsShuttingDown()
{
    auto t = sTimeLine.FindTimer("shutdownTimer");

    return (t && t->isRunning());
}

bool World::CancleShutDown()
{
    auto t = sTimeLine.FindTimer("shutdownTimer");

    if (!t || !t->isRunning())
        return false;

    t->Release();

    cout << "Server shuttingdown cancled..." << endl;

    return true;
}

void World::AddSession(WorldSession *pSession)
{
    m_newSessionQueue.push_back(pSession);
}

void World::_AddSession(WorldSession *pSession)
{
    if (m_sessionMap[pSession->getAccountID()])
        m_sessionMap[pSession->getAccountID()]->KickPlayer();

    SessionMapItr itr = m_sessionMap.find(pSession->getAccountID());

    if (itr != m_sessionMap.end())
        delete itr->second;

    m_sessionMap[pSession->getAccountID()] = pSession;
}

bool World::RemoveSession(WorldSession *pSession)
{
    if (!pSession)
        return false;

    pSession->KickPlayer();

    return true;
}

WorldSession *World::FindSession(uint32 id) const
{
    SessionMapItr itr = m_sessionMap.find(id);

    if (itr != m_sessionMap.end())
        return itr->second;

    return NULL;
}

bool World::KickPlayer(uint32 id)
{
    for (auto s : m_sessionMap)
    {
        if (s.second->getPlayer()->getID() == id)
        {
            s.second->KickPlayer();
            return true;
        }
    }

    return false;
}

void World::KickAllPlayer()
{
    m_queuedSessionQueue.clear();

    for (auto s : m_sessionMap)
        s.second->KickPlayer();

    for (auto s : m_newSessionQueue)
        SAFE_DELETE(s);

    m_newSessionQueue.clear();
}

void World::SendWorldMessage(const string msg, int32 sender)
{
    WorldPacket data(SMSG_CHATMESSAGE, 8 + sizeof(msg));

    data << int32(sender);
    data << msg;

    SendGlobalWorldPacket(data);
}

void World::SendGlobalWorldPacket(WorldPacket &data)
{
    if (m_stopped)
        return;

    for (auto s : m_sessionMap)
        s.second->SendPacket(&data);
}