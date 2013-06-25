#include "MapManager.h"
#include "Map.h"

MapManager::~MapManager()
{
    UnloadAllMaps();
}

void MapManager::Update(const uint32 &diff)
{
    for (auto m : m_mapMap)
        m.second->Update(diff);
}

Map *MapManager::LoadMap(uint32 id)
{
    Map *map = FindMap(id);

    if (map)
        return map;

    map = new Map(id, 999, 30);

    m_mapMap[id] = map;

    return map;
}

Map *MapManager::FindMap(uint32 id) const
{
    for (auto m : m_mapMap)
    {
        if (m.second->getID() == id)
            return m.second;
    }

    return NULL;
}

void MapManager::UnloadAllMaps()
{
    for (auto s : m_mapMap)
        delete s.second;

    m_mapMap.clear();
}