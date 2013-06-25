#ifndef _SH_GAME_CLASS_MAP_H_
#define _SH_GAME_CLASS_MAP_H_

#include <common.h>

#include "Grid.h"

class WorldObject;
class Player;
class WorldPacket;

struct Area
{

};

struct Zone
{

};

class Map
{
    public:
        explicit Map(uint32 id, uint32 gridHCount, uint32 gridVCount): m_id(id), m_gridHCount(gridHCount), m_gridVCount(gridVCount) { }
        virtual ~Map();

        void         Load();
        uint32       getID() const { return m_id; }

        void         Add(Player *pPlayer);
        void         Remove(Player *pPlayer);

        template<typename T>
        void         Add(T obj);

        template<typename T>
        void         Remove(T obj);

        virtual void Update(const uint32 &diff);

        void         MessageBroadcast(Player *pPlayer, WorldPacket *packet, bool self = false);
        void         MessageBroadcast(WorldObject *pObj, WorldPacket *packet);
        void         MessageBroadcastWithDist(WorldObject *pObj, const WorldPacket *packet, float dist);
        void         MessageBroadcastWithDist(Player *pPlayer, const WorldPacket *packet, float dist, bool self = false);
        void         SendMessageToPlayers(WorldPacket *packet);
        void         SendMapMessage(const std::string &msg);
        void         SendZoneMessage(const std::string &msg);
        void         SendAreaMessage(const std::string &msg);

    private:
        Grid        *GetGrid(const uint32 &locX, const uint32 &locY);
        void         GetGridLocation(const float &posX, const float &posY, uint32 &locX, uint32 &locY);
        bool         IsGridLoaded(const uint32 &locX, const uint32 &locY) { return m_grids.find(locY * m_gridHCount + locX) != m_grids.end(); }
        bool         LoadGrid(const float &posX, const float &posY);
        bool         LoadGrid(const uint32 &locX, const uint32 &locY);
        bool         UnloadGrid(const uint32 &locX, const uint32 &locY);

        template<typename T>
        void         AddToGrid(T obj);

        template<typename T>
        void         AddToGrid(const uint32 &locX, const uint32 &locY, T obj);

        template<typename T>
        void         RemoveFromGrid(T obj);

        template<typename T>
        void         RemoveFromGrid(const uint32 &locX, const uint32 &locY, T obj);

        uint32       m_id;
        uint32       m_gridHCount;
        uint32       m_gridVCount;

        std::map<uint32, Grid*>             m_grids;
        ObjectContainer<uint32, Object*>    m_objects;
};

#endif