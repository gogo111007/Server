#include <SHTimer.h>

NAMESPACE_SH

    bool SHTimer::passed(int32 diff)
    {
        if (diff != -1)
        {
            m_elapsedTime          += diff;
            m_elapsedAfterLastTick += diff;
        }

        if (m_elapsedAfterLastTick >= m_interval)
        {
            m_elapsedAfterLastTick -= m_interval;

            if (diff != -1)
                m_currentCount++;

            return true;
        }

        return false;
    }

    void SHTimer::Start()
    {
        if (m_isRunning || m_isReleased)
            return;

        m_isRunning = true;

        if (m_isPaused)
            m_isPaused = false;
        else
            m_prevTickTime = now();
    }

    void SHTimer::Stop()
    {
        m_currentCount         = 0;
        m_isRunning            = false;
        m_isPaused             = false;
        m_elapsedTime          = 0;
        m_elapsedAfterLastTick = 0;
        m_prevTickTime         = from_time_t(0);

        if (m_stopHandler)
            m_stopHandler(this);
    }

    void SHTimer::Pause()
    {
        m_isRunning = false;
        m_isPaused  = true;
    }

    void SHTimer::Restart()
    {
        Stop();
        Start();
    }

    void SHTimer::Release()
    {
        if (m_isPaused || m_isRunning)
            Stop();

        m_tickHandler     = nullptr;
        m_completeHandler = nullptr;
        m_stopHandler     = nullptr;

        m_isReleased      = true;
    }

    void SHTimer::Update(uint32 diff)
    {
        if (!m_isRunning || m_isReleased || !m_tickHandler)
            return;

        m_elapsedTime          += diff;
        m_elapsedAfterLastTick += diff;

        if (passed())
        {
            m_prevTickTime = now();
            m_currentCount++;

            m_tickHandler(this);

            if (m_currentCount >= m_repeatCount && m_repeatCount)
                Complete();
        }
    }

    void SHTimer::Complete()
    {
        m_isRunning = false;
        m_isPaused  = false;

        if (m_completeHandler)
            m_completeHandler(this);

        Release();
    }

NAMESPACE_SH_END