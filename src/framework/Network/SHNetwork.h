#ifndef _SH_FRAMEWORK_CLASS_SHNETWORK_H_
#define _SH_FRAMEWORK_CLASS_SHNETWORK_H_

#include <SHSocket.h>
#include <SHSocketGroup.h>

NAMESPACE_SH

    class SHNetwork
    {
        public:
            SHNetwork(NetType type, Protocol protocol = TCP, IPVersion IPVersion = ANY, const string &host = "localhost", uint32 localPort = 0, uint32 remotePort = 0, uint32 listenQueueCount = DEFAULT_LISTEN_QUEUE);

            ~SHNetwork();

            bool    Init();
            void    Release();

            bool                Initialized() const { return m_pSocket != NULL; }
            SHSocket           *getSocket()   const { return m_pSocket; }
            SHSocketGroup      *getGroup()    const { return m_pGroup;  }

        private:
            SHSocket           *m_pSocket;
            SHSocketGroup      *m_pGroup;

            NetType             m_netType;
            Protocol            m_protocal;
            IPVersion           m_IPVersion;
            string              m_host;
            uint32              m_localPort;
            uint32              m_remotePort;
            uint32              m_listenQueueCount;
    };

NAMESPACE_SH_END

#endif

