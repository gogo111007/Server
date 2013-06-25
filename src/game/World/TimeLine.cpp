#include "TimeLine.h"
#include "Game.h"
#include "World.h"

void TimeLine::Init()
{
}

void TimeLine::Start()
{
    if (m_started)
        return;
    else
        m_started = true;

    auto   prevTickTime  = WorldTimer::now();
    auto   currTickTime  = WorldTimer::from_time_t(0);
    uint32 prevSleepTime = 0;

    while (!sWorld.stopped())
    {
        ++World::m_worldLoopCount;

        currTickTime = WorldTimer::now();

        uint32 diff = WorldTimer::GetTimerDiff(prevTickTime, currTickTime);

        sGame.Update(diff);
        sWorld.Update(diff);
        sTimeLine.Update(diff);

        prevTickTime = currTickTime;

        if (diff <= TIMELINE_SLEEP_INTERVAL + prevSleepTime)
        {
            prevSleepTime = TIMELINE_SLEEP_INTERVAL + prevSleepTime - diff;
            this_thread::sleep_for(milliseconds(prevSleepTime));
        }
        else
            prevSleepTime = 0;
    }

    sTimeLine.Release();
    sWorld.Release();
    sGame.Release();
}

void TimeLine::Release()
{
    ClearTimerList();
}

WorldTimer *TimeLine::FindTimer(string timerName) const
{
    for (auto t : m_timerList)
    {
        if (t && t->getName() == timerName)
            return t;
    }

    for (auto t : m_queuedTimers)
    {
        if (t && t->getName() == timerName)
            return t;
    }

    return NULL;
}

bool TimeLine::AddTimer(WorldTimer *pTimer)
{
    if (!pTimer || IsTimerInList(pTimer))
        return false;

    if (pTimer->getName() != "")
    {
        auto t = FindTimer(pTimer->getName());

        SAFE_RELEASE(t);
    }

    if (m_locked)
        m_queuedTimers.push_back(pTimer);
    else
        m_timerList.push_back(pTimer);

    return true;
}

void TimeLine::RemoveAllTimers()
{
    for (auto t : m_timerList)
        SAFE_RELEASE(t);
}

bool TimeLine::StartTimer(string timerName)
{
    auto t = FindTimer(timerName);

    if (t)
    {
        t->Start();
        return true;
    }

    return false;
}

bool TimeLine::IsTimerInList(WorldTimer *pTimer)
{
    if (!pTimer)
        return false;

    for (auto t : m_timerList)
    {
        if (t == pTimer)
            return true;
    }

    for (auto t : m_queuedTimers)
    {
        if (t == pTimer)
            return true;
    }

    return false;
}

bool TimeLine::StopTimer(string timerName)
{
    auto t = FindTimer(timerName);

    if (t)
    {
        t->Stop();
        return true;
    }

    return false;
}

bool TimeLine::PauseTimer(string timerName)
{
    auto t = FindTimer(timerName);

    if (t)
    {
        t->Pause();
        return true;
    }

    return false;
}

bool TimeLine::RestartTimer(string timerName)
{
    auto t = FindTimer(timerName);

    if (t)
    {
        t->Restart();
        return true;
    }

    return false;
}

void TimeLine::Update(uint32 diff)
{
    for (auto t : m_queuedTimers)
        m_timerList.push_back(t);

    m_queuedTimers.clear();

    m_locked = true;

    auto itr = m_timerList.begin();

    while (itr != m_timerList.end())
    {
        if (*itr && !(*itr)->isReleased())
        {
            (*itr)->Update(diff);
            ++itr;
        }
        else
        {
            SAFE_DELETE(*itr);
            itr = m_timerList.erase(itr);
        }
    }

    m_locked = false;
}

void TimeLine::ClearTimerList()
{
    for (auto t : m_timerList)
        SAFE_RELEASE_DEL(t);

    for (auto t : m_queuedTimers)
        SAFE_RELEASE_DEL(t);

    m_timerList.clear();
    m_queuedTimers.clear();
}