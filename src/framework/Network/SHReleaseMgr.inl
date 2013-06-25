template <typename T>
SHReleaseMgr<T>::SHReleaseMgr(void (*pReleaseFunction)(T*)):
    m_pReleaseFunction(pReleaseFunction)
{
}

template <typename T>
SHReleaseMgr<T>::~SHReleaseMgr()
{
    for (uint32 i = 0; i < (uint32)m_pReleaseQueue.size(); ++i)
    {
        if (*m_pReleaseQueue.at(i))
        {
            if (m_pReleaseFunction)
                m_pReleaseFunction(*m_pReleaseQueue.at(i));
            else
                delete *m_pReleaseQueue.at(i);
        }
    }
}

template <typename T>
void SHReleaseMgr<T>::add(T **ppVar)
{
    m_pReleaseQueue.push_back(ppVar);
}
