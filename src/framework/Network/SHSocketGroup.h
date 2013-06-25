#ifndef _SH_FRAMEWORK_CLASS_SOCKETGROUP_H_
#define _SH_FRAMEWORK_CLASS_SOCKETGROUP_H_

#include <SHNetworkDef.h>
#include <SHSocket.h>

NAMESPACE_SH

    using std::vector;

    class SHSocketGroup;

    class SHSocketEventHandler
    {
        public:
            virtual ~SHSocketEventHandler() { }

            virtual void OnAccept       (SHSocket *pSocket, SHSocketGroup *pGroup, void *pReference) = 0;
            virtual void OnRead         (SHSocket *pSocket, SHSocketGroup *pGroup, void *pReference) = 0;
            virtual void OnDisconnect   (SHSocket *pSocket, SHSocketGroup *pGroup, void *pReference) = 0;
    };

    class SHSocketGroup
    {
        public:
            SHSocketGroup(): m_pSocketEventHandler(NULL) { }

            void      Add(SHSocket *pSocket);
            
            void      Remove(uint32 index);
            void      Remove(SHSocket *pSocket);

            SHSocket               *getSocket(uint32 index)   const;
            SHSocketEventHandler   *getHanlder()              const;
            size_t                  size()                    const;
            void                    setHandler(SHSocketEventHandler *pHandler);

            bool Listen(uint32 milisec = 0, void *pReference = NULL);

        private:
            vector<SHSocket*>        m_socketList;

            SHSocketEventHandler    *m_pSocketEventHandler;
    };

    #include <SHSocketGroup.inl>

NAMESPACE_SH_END

#endif
