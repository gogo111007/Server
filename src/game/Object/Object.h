#ifndef _SH_GAME_CLASS_OBJECT_H_
#define _SH_GAME_CLASS_OBJECT_H_

#include <common.h>

enum ObjectTypeID
{
    TYPEID_OBJECT           = 0,
    TYPEID_ITEM             = 1,
    TYPEID_CONTAINER        = 2,
    TYPEID_UNIT             = 3,
    TYPEID_PLAYER           = 4,
    TYPEID_GAMEOBJECT       = 5,
    TYPEID_DYNAMICOBJECT    = 6,
    TYPEID_CORPSE           = 7,
    TYPEID_UNKNOW           = 8
};

enum ObjectTypeMask
{
    TYPEMASK_OBJECT         = 0x0001,
    TYPEMASK_ITEM           = 0x0002,
    TYPEMASK_CONTAINER      = 0x0004,
    TYPEMASK_UNIT           = 0x0008,
    TYPEMASK_PLAYER         = 0x0010,
    TYPEMASK_GAMEOBJECT     = 0x0020,
    TYPEMASK_DYNAMICOBJECT  = 0x0040,
    TYPEMASK_CORPSE         = 0x0080,

    TYPEMASK_CREATURE_OR_GAMEOBJECT             = TYPEMASK_UNIT | TYPEMASK_GAMEOBJECT,
    TYPEMASK_CREATURE_GAMEOBJECT_OR_ITEM        = TYPEMASK_UNIT | TYPEMASK_GAMEOBJECT | TYPEMASK_ITEM,
    TYPEMASK_CREATURE_GAMEOBJECT_PLAYER_OR_ITEM = TYPEMASK_UNIT | TYPEMASK_GAMEOBJECT | TYPEMASK_PLAYER | TYPEMASK_ITEM,
    TYPEMASK_WORLDOBJECT                        = TYPEMASK_UNIT | TYPEMASK_GAMEOBJECT | TYPEMASK_PLAYER | TYPEMASK_DYNAMICOBJECT | TYPEMASK_CORPSE,
};

struct Position
{
    Position():x(0.0f), y(0.0f), o(0.0f) { }
    Position(float _x, float _y, float _o):x(_x), y(_y), o(_o) { }

    float x, y, o;
};

struct Location
{
    explicit Location(uint32 _map = 0, float _x = 0.0f, float _y = 0.0f, float _o = 0.0f):map(_map), x(_x), y(_y), o(_o) { }
    Location(const Location &loc):map(loc.map), x(loc.x), y(loc.y), o(loc.o) { }

    uint32 map;
    float  x, y, o;
};

class Object
{
    public:
        virtual ~Object() { }

        virtual void  Create(uint32 guid) { m_guid = guid; }
        virtual void  AddToWorld();
        virtual void  RemoveFromWorld();

        const   bool &isInWorld()                        { return m_isInWorld; }
        uint32        getGUID()                    const { return m_guid;      }
        uint8         getTypeID()                  const { return m_typeID;    }
        bool          isType(ObjectTypeMask mask)  const { return (mask & m_typeMask); }

    private:
        bool      m_isInWorld;

    protected:
        explicit Object():m_isInWorld(false), m_guid(0), m_typeMask(TYPEMASK_OBJECT), m_typeID(TYPEID_OBJECT) { }

        uint32  m_guid;
        uint8   m_typeID;
        uint16  m_typeMask;
};

#endif