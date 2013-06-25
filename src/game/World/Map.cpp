#include "Map.h"
#include "Player.h"
#include "WorldPacket.h"

Map::~Map()
{

}

void Map::Load()
{
}

void Map::Update(const uint32 &diff)
{
    for (auto itr = m_objects.begin(); itr != m_objects.end();)
    {
        if (itr->second)
        {
            if (itr->second->getTypeID() == TYPEID_PLAYER)
            {
                Player *pPlayer = (Player*)(itr->second);

                pPlayer->Update(diff);
            }
            ++itr;
        }
        else
            itr = m_objects.erase(itr);
    }

    for (auto itr = m_grids.begin();itr != m_grids.end();)
    {
        if (itr->second)
            itr->second->Update(diff);

        if (!itr->second || itr->second->getUnloadTimer() >= 5000)
        {
            SAFE_DELETE(itr->second);
            std::cout << "Grid " << itr->first << " unloaded!" << std::endl;
            itr = m_grids.erase(itr);
        }
        else
            ++itr;
    }
}

void Map::Add(Player *pPlayer)
{
    if (!pPlayer)
        return;

    uint32 gridX = 0;
    uint32 gridY = 0;

    GetGridLocation(pPlayer->getPositionX(), pPlayer->getPositionY(), gridX, gridY);

    LoadGrid(gridX, gridY);
    AddToGrid(gridX, gridY, pPlayer);

    pPlayer->setMap(this);
}

void Map::Remove(Player *pPlayer)
{
    if (!pPlayer)
        return;

    RemoveFromGrid(pPlayer);
}

void Map::SendMessageToPlayers(WorldPacket *packet)
{

}

void Map::SendMapMessage(const std::string &msg)
{

}

void Map::SendZoneMessage(const std::string &msg)
{

}

void Map::SendAreaMessage(const std::string &msg)
{

}

void Map::MessageBroadcast(Player *pPlayer, WorldPacket *packet, bool self)
{

}

void Map::MessageBroadcast(WorldObject *pObj, WorldPacket *packet)
{

}

void Map::MessageBroadcastWithDist(WorldObject *pObj, const WorldPacket *packet, float dist)
{

}

void Map::MessageBroadcastWithDist(Player *pPlayer, const WorldPacket *packet, float dist, bool self)
{

}


template<typename T>
void Map::Add(T obj)
{
    
}

template<typename T>
void Map::Remove(T obj)
{

}

template<typename T>
void Map::AddToGrid(T obj)
{

}

template<typename T>
void Map::AddToGrid(const uint32 &locX, const uint32 &locY, T obj)
{
    m_objects[obj->getGUID()] = obj;

    if (!IsGridLoaded(locX, locY))
        return;

    GetGrid(locX, locY)->Add(obj);
}

template<typename T>
void Map::RemoveFromGrid(T obj)
{
    uint32 gridX = 0;
    uint32 gridY = 0;

    GetGridLocation(obj->getPositionX(), obj->getPositionY(), gridX, gridY);

    RemoveFromGrid(gridX, gridY, obj);
}

template<typename T>
void Map::RemoveFromGrid(const uint32 &locX, const uint32 &locY, T obj)
{
    m_objects[obj->getGUID()] = nullptr;

    if (IsGridLoaded(locX, locY))
        GetGrid(locX, locY)->Remove(obj);
}

void Map::GetGridLocation(const float &posX, const float &posY, uint32 &locX, uint32 &locY)
{
    Grid::CalculateGridLocation(posX, posY, locX, locY);

    if (locX >= m_gridHCount)
        locX = m_gridHCount - 1;
    if (locY >= m_gridVCount)
        locY = m_gridVCount - 1;
}

Grid *Map::GetGrid(const uint32 &locX, const uint32 &locY)
{
    if (IsGridLoaded(locX, locY))
        return m_grids[locY * m_gridHCount + locX];
    else
        return nullptr;
}

bool Map::LoadGrid(const float &posX, const float &posY)
{
    uint32 gridX = 0;
    uint32 gridY = 0;

    GetGridLocation(posX, posY, gridX, gridY);

    return LoadGrid(gridX, gridY);
}

bool Map::LoadGrid(const uint32 &locX, const uint32 &locY)
{
    if (IsGridLoaded(locX, locY))
        return false;

    m_grids[locY * m_gridHCount + locX] = new Grid(locX, locY);

    std::cout << "Grid " << locY * m_gridHCount + locX << " loaded!" << std::endl;

    return true;
}

bool Map::UnloadGrid(const uint32 &locX, const uint32 &locY)
{
    auto itr = m_grids.find(locY * m_gridHCount + locX);

    if (itr == m_grids.end())
        return false;

    SAFE_DELETE(itr->second);

    return true;
}