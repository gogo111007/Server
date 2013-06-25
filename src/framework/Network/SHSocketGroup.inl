inline void SHSocketGroup::Add(SHSocket *pSocket)
{
    pSocket->Parent(this);

    m_socketList.push_back(pSocket);
}

inline SHSocket *SHSocketGroup::getSocket(uint32 index) const
{
    if (index >= m_socketList.size())
        throw SHException(SHException::SH_EXCEPTION_OUT_OF_RANGE, "SHSocketGroup::get: index out of range");

    return m_socketList[index];
}

inline void SHSocketGroup::Remove(uint32 index)
{
    if (index >= m_socketList.size())
        throw SHException(SHException::SH_EXCEPTION_OUT_OF_RANGE, "SHSocketGroup::remove: index out of range");

    m_socketList.erase(m_socketList.begin() + index);
}

inline size_t SHSocketGroup::size() const
{
    return m_socketList.size();
}

inline SHSocketEventHandler *SHSocketGroup::getHanlder() const
{
    return m_pSocketEventHandler;
}

inline void SHSocketGroup::setHandler(SHSocketEventHandler *pHandler)
{
    m_pSocketEventHandler = pHandler;
}
