#ifndef _SH_FRAMEWORK_CLASS_SHTIMER_H_
#define _SH_FRAMEWORK_CLASS_SHTIMER_H_

#include <common.h>

NAMESPACE_SH

    NAMESPACE_STD_USE

    using namespace std::chrono;

    class SHTimer: public high_resolution_clock
    {
        public:
            typedef function<void (void*)> handle;

            SHTimer(uint32 interval = 0, uint32 repeatCount = 0, handle tickHandler = nullptr, handle completeHandler = nullptr, handle stopHandler = nullptr, bool autoStart = false):
                m_isRunning(false), m_isPaused(false), m_isReleased(false), m_interval(interval), m_repeatCount(repeatCount), m_currentCount(0), m_prevTickTime(from_time_t(0)),
                m_elapsedTime(0), m_elapsedAfterLastTick(0),
                m_tickHandler(tickHandler), m_completeHandler(completeHandler), m_stopHandler(stopHandler)
            {
                if (autoStart)
                    Start();
            }

            virtual ~SHTimer() { }

            virtual void Update(uint32 diff);
            virtual void Start();
            virtual void Stop();
            virtual void Pause();
            virtual void Restart();
            virtual void Release();

            bool         passed(int32 diff = -1);

            bool         isRunning()           const               { return m_isRunning;       }
            bool         isPaused()            const               { return m_isPaused;        }
            bool         isReleased()          const               { return m_isReleased;      }
            uint32       getElapsedTime()      const               { return m_elapsedTime;     }
            uint32       getInterval()         const               { return m_interval;        }
            uint32       getCurrentCount()     const               { return m_currentCount;    }
            uint32       getRepeatCount()      const               { return m_repeatCount;     }
            handle       getTickHandler()      const               { return m_tickHandler;     }
            handle       getCompleteHandler()  const               { return m_completeHandler; }
            handle       getStopHandler()      const               { return m_stopHandler;     }
            time_point   getPrevTickTime()     const               { return m_prevTickTime;    }

            void         setInterval         (uint32 interval)     { m_interval         = interval;    }
            void         setRepeatCount      (uint32 repeatCount)  { m_repeatCount      = repeatCount; }
            void         setTickHandler      (handle handler)      { m_tickHandler      = handler;     }
            void         setCompleteHandler  (handle handler)      { m_completeHandler  = handler;     }
            void         setStopHandler      (handle handler)      { m_stopHandler      = handler;     }

            static inline uint32 GetTimerDiff(const time_point &prev, const time_point &curr)
            {
                auto prevMSTime = DurationConvert<milliseconds>(prev.time_since_epoch()).count();
                auto currMSTime = DurationConvert<milliseconds>(curr.time_since_epoch()).count();

                if (prevMSTime > currMSTime)
                    return min(uint32((uint64(0xFFFFFFFFFFFFFFFF) - prevMSTime) + currMSTime), uint32(prevMSTime - currMSTime));
                else
                    return uint32(currMSTime - prevMSTime);
            }

            template<typename T>
            static inline T DurationConvert(const duration &d)
            {
                return duration_cast<T>(d);
            }

        protected:
            virtual void Complete();

            bool         m_isRunning;
            bool         m_isPaused;
            bool         m_isReleased;
            uint32       m_interval;
            uint32       m_elapsedTime;
            uint32       m_elapsedAfterLastTick;
            uint32       m_repeatCount;
            uint32       m_currentCount;

            handle       m_tickHandler;
            handle       m_completeHandler;
            handle       m_stopHandler;

            time_point   m_prevTickTime;
    };

NAMESPACE_SH_END

#endif
