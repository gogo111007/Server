#include <SHSocket.h>
#include <SHSocketGroup.h>

#include <string.h>
#include <stdio.h>

NAMESPACE_SH

    #if PLATFORM == PLATFORM_WINDOWS
        static void close(int32 socketHandler)
        {
            closesocket(socketHandler);
        }

        static void freeaddrinfo(PADDRINFOA addrInfo)
        {
            ::freeaddrinfo(addrInfo);
        }

        static const char *inet_ntop(int32 af, const void *pSrc, char *pDst, socklen_t cnt)
        {
            if (af == AF_INET)
            {
                struct sockaddr_in in;
                memset(&in, 0, sizeof(in));
                in.sin_family = AF_INET;
                memcpy(&in.sin_addr, pSrc, sizeof(in_addr));
                getnameinfo((struct sockaddr *)&in, sizeof(sockaddr_in), pDst, cnt, NULL, 0, NI_NUMERICHOST);
                return pDst;
            }
            else if (af == AF_INET6)
            {
                struct sockaddr_in6 in;
                memset(&in, 0, sizeof(in));
                in.sin6_family = AF_INET6;
                memcpy(&in.sin6_addr, pSrc, sizeof(in_addr6));
                getnameinfo((struct sockaddr *)&in, sizeof(sockaddr_in6), pDst, cnt, NULL, 0, NI_NUMERICHOST);
                return pDst;
            }

            return NULL;
        }
    #endif

    static uint32 getInPort(struct sockaddr *pSa)
    {
        if (pSa->sa_family == AF_INET)
            return ntohs(((struct sockaddr_in*)pSa)->sin_port);

        return ntohs(((struct sockaddr_in6*)pSa)->sin6_port);
    }

    static int32 getSocketErrorCode()
    {
        #if PLATFORM == PLATFORM_WINDOWS
            return WSAGetLastError();
        #else
            return errno;
        #endif
    }

    static uint32 getLocalPort(int32 socketHandler)
    {
        struct sockaddr_storage sin;

        #if PLATFORM == PLATFORM_WINDOWS
            int32 size;
        #else
            socklen_t size;
        #endif

        size = sizeof(sin);
        if (getsockname(socketHandler, (struct sockaddr*)&sin, &size) == 0)
            return getInPort((struct sockaddr*)&sin);
        else
            throw SHException(SHException::SH_EXCEPTION_NETWORK_GET_ADDR_INFO, "SHSocket::(static)getLocalPort: error getting socket info", getSocketErrorCode());
    }

    static void checkReadError(const string &functionName)
    {
        #if PLATFORM == PLATFORM_WINDOWS
            if (WSAGetLastError() != WSAEWOULDBLOCK)
                throw SHException(SHException::SH_EXCEPTION_NETWORK_READ, string("SHSocket::") + functionName + ": error detected", getSocketErrorCode());
        #else
            if (errno != EAGAIN && errno != EWOULDBLOCK)
                throw SHException(SHException::SH_EXCEPTION_NETWORK_READ, string("SHSocket::") + functionName + ": error detected", getSocketErrorCode());
        #endif
    }

    void SHSocket::InitSocket()
    {
        struct addrinfo conf, *pRes = NULL;
        memset(&conf, 0, sizeof(conf));

        if (m_type == SERVER || m_protocol == UDP)
            conf.ai_flags = AI_PASSIVE;

        switch(m_protocol)
        {
            case TCP:
                conf.ai_socktype = SOCK_STREAM;
                break;
            case UDP:
                conf.ai_socktype = SOCK_DGRAM;
                break;
            default:
                throw SHException(SHException::SH_EXCEPTION_NETWORK_BAD_PROTOCOL, "SHSocket::InitSocket: bad protocol");
        }

        switch(m_IPVersion)
        {
            case IPv4:
                conf.ai_family = AF_INET;
                break;
            case IPv6:
                conf.ai_family = AF_INET6;
                break;
            case ANY:
                conf.ai_family = AF_UNSPEC;
                break;
            default:
                throw SHException(SHException::SH_EXCEPTION_NETWORK_BAD_IP_VER, "SHSocket::InitSocket: bad ip version parameter");
        }

        char portStr[10];

        const char *pHost;

        if (m_type == CLIENT && m_protocol == TCP)
        {
            pHost = m_remoteHost.c_str();
            snprintf(portStr, 10, "%u", m_remotePort);
        }
        else
        {
            if (!m_localHost.compare("") || !m_localHost.compare("*"))
                pHost = NULL;
            else
                pHost = m_localHost.c_str();

            snprintf(portStr, 10, "%u", m_localPort);
        }

        int32 status = getaddrinfo(pHost, portStr, &conf, &pRes);

        SHReleaseMgr<struct addrinfo> addrInfoReleaser(freeaddrinfo);
        addrInfoReleaser.add(&pRes);

        if (status != 0)
        {
            string errorMsg = "SHSocket::InitSocket: Error setting addrInfo: ";

            #ifndef _MSC_VER
                errorMsg += gai_strerror(status);
            #endif

            throw SHException(SHException::SH_EXCEPTION_NETWORK_SET_ADDR_INFO, errorMsg, getSocketErrorCode());
        }

        bool connected = false;

        while (!connected && pRes)
        {
            m_socketHandler = socket(pRes->ai_family, pRes->ai_socktype, pRes->ai_protocol);

            if (m_socketHandler != -1)
            {
                switch(m_type)
                {
                    case CLIENT:
                    {
                        if (m_protocol == UDP)
                        {

                            if (bind(m_socketHandler, pRes->ai_addr, pRes->ai_addrlen) == -1)
                                close(m_socketHandler);
                            else
                                connected = true;
                        }
                        else
                        {
                            status = connect(m_socketHandler, pRes->ai_addr, pRes->ai_addrlen);
                            if (status != -1)
                                connected = true;
                            else
                                close(m_socketHandler);
                        }
                        break;
                    }
                    case SERVER:
                    {
                        #if PLATFORM == PLATFORM_WINDOWS
                            char optVal = 1;
                        #else
                            int32 optVal = 1;
                        #endif

                        if (setsockopt(m_socketHandler, SOL_SOCKET, SO_REUSEADDR, &optVal, sizeof(int32)) == -1)
                            throw SHException(SHException::SH_EXCEPTION_NETWORK_SET_SOCK_OPT, "SHSocket::InitSocket: Error establishing socket options");

                        if (bind(m_socketHandler, pRes->ai_addr, pRes->ai_addrlen) == -1)
                            close(m_socketHandler);
                        else
                            connected = true;

                        if (m_protocol == TCP && listen(m_socketHandler, m_listenQueue) == -1)
                            throw SHException(SHException::SH_EXCEPTION_NETWORK_CAN_NOT_LISTEN, "SHSocket::InitSocket: could not start listening", getSocketErrorCode());

                        break;
                    }
                }
            }

            if (connected && m_IPVersion == ANY)
            {
                switch(pRes->ai_family)
                {
                    case AF_INET:
                        m_IPVersion = IPv4;
                        break;

                    case AF_INET6:
                        m_IPVersion = IPv6;
                        break;
                }
            }

            pRes = pRes->ai_next;
        }

        if (!connected)
            throw SHException(SHException::SH_EXCEPTION_NETWORK_CONNECT_SOCKET, "SHSocket::InitSocket: error in socket connection/bind", getSocketErrorCode());

        if (!m_localPort)
            m_localPort = getLocalPort(m_socketHandler);
    }

    SHSocket::SHSocket(const string &host, uint32 port, Protocol protocol, IPVersion IPVersion):
        m_remoteHost(host), m_remotePort(port), m_localPort(0), m_protocol(protocol),
        m_IPVersion(IPVersion), m_type(CLIENT), m_blocking(true), m_listenQueue(0), m_pParent(NULL)
    {
        InitSocket();
    }

    SHSocket::SHSocket(uint32 port, Protocol protocol, IPVersion IPVersion, const string &host, uint32 listenQueue):
        m_localHost(host), m_remotePort(0), m_localPort(port), m_protocol(protocol), m_IPVersion(IPVersion), m_type(SERVER),
        m_blocking(true), m_listenQueue(listenQueue), m_pParent(NULL)
    {
        InitSocket();
    }

    SHSocket::SHSocket(const string &host, uint32 localPort, uint32 remotePort, IPVersion IPVersion):
        m_remoteHost(host), m_remotePort(remotePort), m_localPort(localPort), m_protocol(UDP),
        m_IPVersion(IPVersion), m_type(CLIENT), m_blocking(true), m_listenQueue(0), m_pParent(NULL)
    {
        InitSocket();
    }


    SHSocket::SHSocket():
        m_blocking(true), m_socketHandler(-1)
    {
    }

    SHSocket::~SHSocket()
    {
        if (m_socketHandler != -1)
            close(m_socketHandler);
    }

    static void *get_in_addr(struct sockaddr *pSa)
    {
        if (pSa->sa_family == AF_INET)
        {
            return &(((struct sockaddr_in*)pSa)->sin_addr);
        }
        return &(((struct sockaddr_in6*)pSa)->sin6_addr);
    }

    SHSocket *SHSocket::Accept()
    {
        if (m_protocol != TCP)
            throw SHException(SHException::SH_EXCEPTION_NETWORK_EXPECTED_TCP_SOCKET, "SHSocket::Accept: non-tcp socket can not Accept connections");

        if (m_type != SERVER)
            throw SHException(SHException::SH_EXCEPTION_NETWORK_EXPECTED_SERVER_SOCKET, "SHSocket::Accept: non-server socket can not Accept connections");

        struct sockaddr_storage incoming_addr;

        #if PLATFORM == PLATFORM_WINDOWS
            int32 addrSize = sizeof(incoming_addr);
        #else
            uint32 addrSize = sizeof(incoming_addr);
        #endif

        int32 new_handler = ::accept(m_socketHandler, (struct sockaddr*)&incoming_addr, &addrSize);

        if (new_handler == -1)
            return NULL;

        char hostChar[INET6_ADDRSTRLEN];
        inet_ntop(incoming_addr.ss_family, get_in_addr((struct sockaddr*)&incoming_addr), hostChar, sizeof hostChar);

        SHSocket *pAcceptSocket           = new SHSocket();
        pAcceptSocket->m_socketHandler    = new_handler;
        pAcceptSocket->m_remoteHost       = hostChar;
        pAcceptSocket->m_remotePort       = getInPort((struct sockaddr *)&incoming_addr);
        pAcceptSocket->m_localPort        = getLocalPort(pAcceptSocket->m_socketHandler);
        pAcceptSocket->m_protocol         = m_protocol;
        pAcceptSocket->m_IPVersion        = m_IPVersion;
        pAcceptSocket->m_type             = CLIENT;
        pAcceptSocket->m_listenQueue      = 0;
        pAcceptSocket->Blocking(m_blocking);

        return pAcceptSocket;
    }

    void SHSocket::SendTo(const void *pBuffer, size_t size, const string &remoteHost, uint32 remotePort)
    {
        if (m_protocol != UDP)
            throw SHException(SHException::SH_EXCEPTION_NETWORK_EXPECTED_UDP_SOCKET, "SHSocket::SendTo: non-UDP socket can not 'SendTo'");

        struct addrinfo conf, *pRes;
        memset(&conf, 0, sizeof(conf));

        conf.ai_socktype = SOCK_DGRAM;

        switch(m_IPVersion)
        {
            case IPv4:
                conf.ai_family = AF_INET;
                break;
            case IPv6:
                conf.ai_family = AF_INET6;
                break;
            default:
                throw SHException(SHException::SH_EXCEPTION_NETWORK_BAD_IP_VER, "SHSocket::SendTo: bad ip version.");
        }

        char portStr[10];
        snprintf(portStr, 10, "%u", remotePort);

        int32 status = getaddrinfo(remoteHost.c_str(), portStr, &conf, &pRes);

        SHReleaseMgr<struct addrinfo> addrInfoReleaser(freeaddrinfo);
        addrInfoReleaser.add(&pRes);

        if (status != 0)
        {
            string errorMsg = "SHSocket::SendTo: error setting addrInfo: ";
            #ifndef _MSC_VER
                errorMsg += gai_strerror(status);
            #endif
            throw SHException(SHException::SH_EXCEPTION_NETWORK_SET_ADDR_INFO, "SHSocket::SendTo: error setting addr info", getSocketErrorCode());
        }

        size_t sentBytes = 0;

        while (sentBytes < size)
        {
            int32 status = ::sendto(m_socketHandler, (const char*)pBuffer + sentBytes, size - sentBytes, 0, pRes->ai_addr, pRes->ai_addrlen);

            if (status == -1)
                throw SHException(SHException::SH_EXCEPTION_NETWORK_SEND, "SHSocket::SendTo: could not Send the data", getSocketErrorCode());

            sentBytes += status;
        }
    }

    int32 SHSocket::ReadFrom(void *pBuffer, size_t bufferSize, string *localHost, uint32 *localPort)
    {
        if (m_protocol != UDP)
            throw SHException(SHException::SH_EXCEPTION_NETWORK_EXPECTED_UDP_SOCKET, "SHSocket::ReadFrom: non-UDP socket can not 'ReadFrom'");

        struct sockaddr_storage addr;
        socklen_t addrSize = sizeof(addr);
        int32 status = recvfrom(m_socketHandler, (char*)pBuffer, bufferSize, 0, (struct sockaddr *)&addr, &addrSize);

        if (status == -1)
        {
            checkReadError("ReadFrom");
            if (localHost)
                *localHost = "";
            if (localPort)
                *localPort = 0;
        }
        else
        {
            if (localPort)
                *localPort = getInPort((struct sockaddr*)&addr);

            if (localHost)
            {
                char hostChar[INET6_ADDRSTRLEN];
                inet_ntop(addr.ss_family, get_in_addr((struct sockaddr *)&addr), hostChar, sizeof hostChar);

                *localHost = hostChar;
            }
        }

        return status;
    }

    void SHSocket::Send(const void *pBuffer, size_t size)
    {
        if (m_type != CLIENT)
            throw SHException(SHException::SH_EXCEPTION_NETWORK_EXPECTED_CLIENT_SOCKET, "SHSocket::Send: Expected client socket (socket with host and port target)");

        if (m_protocol == UDP)
            return SendTo(pBuffer, size, m_remoteHost, m_remotePort);

        size_t sentData = 0;

        while (sentData < size)
        {
            int32 status = ::send(m_socketHandler, (const char*)pBuffer + sentData, size - sentData, 0);

            if (status == -1)
                throw SHException(SHException::SH_EXCEPTION_NETWORK_SEND, "Error Sending data", getSocketErrorCode());

            sentData += status;
        }
    }

    int32 SHSocket::Read(void *pBuffer, size_t bufferSize)
    {
        int32 status = recv(m_socketHandler, (char*)pBuffer, bufferSize, 0);

        if (status == -1)
            checkReadError("Read");

        return status;
    }

    int32 SHSocket::nextReadSize() const
    {
        #if PLATFORM == PLATFORM_WINDOWS
            u_long result = -1;
        #else
            int64 result = -1;
        #endif

        int32 status = 0;

        #if PLATFORM == PLATFORM_WINDOWS
            status = ioctlsocket(m_socketHandler, FIONREAD, &result);
        #else
            status = ioctl(_socketHandler, FIONREAD, &result);
        #endif

        if (status)
            throw SHException(SHException::SH_EXCEPTION_NETWORK_IOCTL, "SHSocket::nextReadSize: error ioctl", getSocketErrorCode());

        return result;
    }

    void SHSocket::Blocking(bool blocking)
    {
        m_blocking = blocking;

        int32 result = -1;

        #if PLATFORM == PLATFORM_WINDOWS
            u_long non_blocking = !blocking;
            result = ioctlsocket(m_socketHandler, FIONBIO, &non_blocking);
            if (result != 0)
                result = -1;
        #else
            int32 flags = fcntl(_socketHandler, F_GETFL);

            if (blocking)
                result = fcntl(_socketHandler, F_SETFL, flags & ~O_NONBLOCK);
            else
                result = fcntl(_socketHandler, F_SETFL, flags | O_NONBLOCK);
        #endif

        if (result == -1)
            throw SHException(SHException::SH_EXCEPTION_NETWORK_IOCTL, "SHSocket::blocking: ioctl error", getSocketErrorCode());
    }

    void SHSocket::Disconnect()
    {
        close(m_socketHandler);

        m_socketHandler = -1;

        // must at the end of function Disconnect() because sometimes we may delete this socket in OnDisconnect().
        if ((SHSocketGroup*)m_pParent)
            ((SHSocketGroup*)m_pParent)->getHanlder()->OnDisconnect(this, (SHSocketGroup*)m_pParent, NULL);
    }

NAMESPACE_SH_END
