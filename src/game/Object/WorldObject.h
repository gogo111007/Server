#ifndef _SH_GAME_CLASS_WORLDOBJECT_H_
#define _SH_GAME_CLASS_WORLDOBJECT_H_

#include "Object.h"
#include "Map.h"

class WorldObject : public Object
{
    public:
        virtual ~WorldObject() { m_pMap = nullptr; }

        virtual void    Update(const uint32 &diff);
        virtual void    Create(std::string name, uint32 guid);
        virtual void    RemoveFromWorld();

        void            GetPosition(float &posX, float &posY) const { posX = m_position.x; posY = m_position.y; }
        void            GetPosition(Position &pos)            const { pos.x = m_position.x; pos.y = m_position.y; pos.o = m_position.o; }
        void            GetLocation(Location &loc)            const { loc.map = m_mapID; loc.x = m_position.x; loc.y = m_position.y; loc.o = m_position.o; }

        void            ReLocate(const float &x, const float &y, const float &o);
        void            ReLocate(const float &x, const float &y);
        void            ReLocate(const Location &loc);

        const Position &getPosition()           { return m_position;   }
        Map            *getMap()         const  { return m_pMap;       }
        uint32          getMapID()       const  { return m_mapID;      }
        float           getPositionX()   const  { return m_position.x; }
        float           getPositionY()   const  { return m_position.y; }
        float           getOrientation() const  { return m_position.o; }
        std::string     getName()        const  { return m_name;       }

        void            setMap(Map *pMap)                        { m_pMap = pMap; m_mapID = pMap->getID(); }
        void            setPosition(float x, float y, float o)   { m_position.x = x; m_position.y = y; m_position.o = o; }
        void            setPositionX(float x)                    { m_position.x = x; }
        void            setPositionY(float y)                    { m_position.y = y; }
        void            setOrientation(float o)                  { m_position.o = o; }

    private:
        uint32       m_mapID;
        Position     m_position;
        Map         *m_pMap;

    protected:
        explicit WorldObject():Object(), m_name("NULL"), m_mapID(0), m_position(Position()), m_pMap(nullptr) { }

        std::string  m_name;

};

#endif