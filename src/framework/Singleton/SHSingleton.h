#ifndef _SH_FRAMEWORK_CLASS_SHSINGLETON_H_
#define _SH_FRAMEWORK_CLASS_SHSINGLETON_H_

#include <common.h>

NAMESPACE_SH

    template<typename T>
    class SHSingleton
    {
        public:
            static T &instance();

        protected:
            SHSingleton() { }

        private:
            SHSingleton(const SHSingleton &);
            SHSingleton &operator=(const SHSingleton &);

            static T *m_pInstance;

            static std::once_flag m_onceFlag;
    };

    template<typename T>
    T *SHSingleton<T>::m_pInstance = nullptr;

    template<typename T>
    std::once_flag SHSingleton<T>::m_onceFlag;

    template<typename T>
    T &SHSingleton<T>::instance()
    {
        std::call_once(m_onceFlag, [] { m_pInstance = new T(); });

        return *m_pInstance;
    }

NAMESPACE_SH_END

#endif