#include <SHNetwork.h>

NAMESPACE_SH

    NAMESPACE_STD_USE

    SHNetwork::SHNetwork(NetType type, Protocol protocol, IPVersion IPVersion, const string &host, uint32 localPort, uint32 remotePort, uint32 listenQueueCount):
        m_netType(type), m_protocal(protocol), m_IPVersion(IPVersion), m_host(host), m_remotePort(remotePort), m_localPort(localPort), m_listenQueueCount(listenQueueCount)
    {
        m_pSocket = NULL;
        m_pGroup  = NULL;
    }

    SHNetwork::~SHNetwork()
    {
    }

    bool SHNetwork::Init()
    {
        try
        {
            if (Initialized())
                throw SHException(SHException::SH_EXCEPTION_NETWORK_INIT, "Network already initialized!");

            #if PLATFORM == PLATFORM_WINDOWS
                WSADATA wsaData;
                if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
                    throw SHException(SHException::SH_EXCEPTION_NETWORK_INIT, "Network inicialization failed!");
            #endif

            if (m_netType == SERVER)
                m_pSocket = new SHSocket(m_localPort, m_protocal, m_IPVersion, m_host, m_listenQueueCount);
            else
            {
                if (m_protocal == TCP || (m_protocal == UDP && m_localPort == 0))                // Use random localPort if localPort == 0 when protocal == UDP
                    m_pSocket = new SHSocket(m_host, m_remotePort, m_protocal, m_IPVersion);
                else
                    m_pSocket = new SHSocket(m_host, m_remotePort, m_localPort, m_IPVersion);
            }
        }
        catch (SHException e)
        {
            cout << e.what() << endl;
            return false;
        }
        
        m_pGroup = new SHSocketGroup();

        m_pGroup->Add(m_pSocket);

        return (m_pSocket != NULL && m_pGroup != NULL);
    }

    void SHNetwork::Release()
    {
        #if PLATFORM == PLATFORM_WINDOWS
            WSACleanup();
        #endif

        SAFE_DELETE(m_pGroup);
        SAFE_DELETE(m_pSocket);
    }

NAMESPACE_SH_END
