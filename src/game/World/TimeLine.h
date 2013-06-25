#ifndef _SH_GAME_CLASS_TIMELINE_H_
#define _SH_GAME_CLASS_TIMELINE_H_

#include <common.h>
#include <SHFramework.h>

NAMESPACE_SH_USE
NAMESPACE_STD_USE

#define TIMELINE_SLEEP_INTERVAL 50

class WorldTimer;

class TimeLine
{
    friend class WorldTimer;

    public:
        TimeLine():m_started(false), m_locked(false) { }
        virtual ~TimeLine() { }

        void        Init();
        void        Start();
        void        Release();

        void        RemoveAllTimers();
        bool        StartTimer(string timerName);
        bool        StopTimer(string timerName);
        bool        PauseTimer(string timerName);
        bool        RestartTimer(string timerName);
        bool        IsTimerInList(WorldTimer *pTimer);

        inline bool started() const { return m_started; }

        WorldTimer *FindTimer(string timerName) const;

    protected:
        bool        AddTimer(WorldTimer *pTimer);
        void        ClearTimerList();
        void        Update(uint32 diff);

    private:
        bool        m_started;
        bool        m_locked;

        vector<WorldTimer*> m_timerList;
        deque<WorldTimer*>  m_queuedTimers;
};

#define sTimeLine SHSingleton<TimeLine>::instance()

class WorldTimer: public SHTimer
{
    public:
        WorldTimer(uint32 interval = 0, uint32 repeatCount = 0, handle tickHandler = nullptr, string name = "", handle completeHandler = nullptr, handle stopHandler = nullptr, bool autoStart = false):
            SHTimer(interval, repeatCount, tickHandler, completeHandler, stopHandler, autoStart), m_name(name)
        {
            sTimeLine.AddTimer(this);
        }

        virtual ~WorldTimer() { }

        string getName() const { return m_name; }

    private:
        string  m_name;
};

#endif