#include "Grid.h"
#include "Player.h"

void Grid::Add(Player *pPlayer)
{
    if (!pPlayer)
        return;

    m_objects[pPlayer->getGUID()] = pPlayer;
    m_unloadTimer = 0;
}

void Grid::Remove(Player *pPlayer)
{
    if (!pPlayer)
        return;

    auto p = m_objects.find(pPlayer->getGUID());

    if (p != m_objects.end())
        m_objects.erase(p);
}

template<typename T>
void Grid::Add(T obj)
{
    if (!obj)
        return;

    m_objects[obj->getGUID()] = obj;
    m_unloadTimer = 0;
}

template<typename T>
void Grid::Remove(T obj)
{
    if (!obj)
        return;

    auto p = m_objects.find(obj->getGUID());

    if (p != m_objects.end())
        m_objects.erase(p);
}