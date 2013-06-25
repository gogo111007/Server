#ifndef _SH_FRAMEWORK_CLASS_RELEASEMGR_H_
#define _SH_FRAMEWORK_CLASS_RELEASEMGR_H_

#include <common.h>

NAMESPACE_SH

    using std::vector;

    template<typename T>
    class SHReleaseMgr
    {
        protected:
            vector<T**>   m_pReleaseQueue;
            void        (*m_pReleaseFunction) (T*);

        public:
            SHReleaseMgr(void (*pReleaseFunction)(T*) = NULL);
            virtual ~SHReleaseMgr();

            void add(T **ppVar);
    };

    #include <SHReleaseMgr.inl>

NAMESPACE_SH_END

#endif
