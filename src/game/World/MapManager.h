#ifndef _SH_GAME_CLASS_MAPMANAGER_H_
#define _SH_GAME_CLASS_MAPMANAGER_H_

#include <common.h>
#include <SHFramework.h>

class Map;

NAMESPACE_SH_USE
NAMESPACE_STD_USE

class MapManager
{
    typedef map<uint32, Map*> MapMap;

    public:
        MapManager() { }
        ~MapManager();

        void            Update(const uint32 &diff);

        Map            *LoadMap(uint32 id);
        Map            *FindMap(uint32 id) const;

        const MapMap   &GetMaps() const { return m_mapMap; }

        void            UnloadAllMaps();

    private:
        MapMap   m_mapMap;
};

#define sMapManager SHSingleton<MapManager>::instance()

#endif