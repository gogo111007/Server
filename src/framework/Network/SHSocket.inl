inline bool SHSocket::disconnected() const
{
    return m_socketHandler == -1;
}

inline const string &SHSocket::remoteHost() const
{
    return m_remoteHost;
}

inline const string &SHSocket::localHost() const
{
    return m_localHost;
}

inline uint32 SHSocket::remotePort() const
{
    return m_remotePort;
}

inline uint32 SHSocket::localPort() const
{
    return m_localPort;
}

inline Protocol SHSocket::protocol() const
{
    return m_protocol;
}

inline IPVersion SHSocket::ipVersion() const
{
    return m_IPVersion;
}

inline NetType SHSocket::type() const
{
    return m_type;
}

inline uint32 SHSocket::listenQueue() const
{
    return m_listenQueue;
}

inline bool SHSocket::blocking() const
{
    return m_blocking;
}

inline int32 SHSocket::socketHandler() const
{
    return m_socketHandler;
}

inline void SHSocket::Parent(void *pParent)
{
    m_pParent = pParent;
}
inline void *SHSocket::parent() const
{
    return m_pParent;
}