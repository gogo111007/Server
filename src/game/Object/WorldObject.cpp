#include "WorldObject.h"

void WorldObject::Update(const uint32 &diff)
{

}

void WorldObject::Create(std::string name, uint32 guid)
{
    Object::Create(guid);

    m_name = name;
}

void WorldObject::RemoveFromWorld()
{
    Object::RemoveFromWorld();
}

void WorldObject::ReLocate(const float &x, const float &y, const float &o)
{
    m_position.x = x;
    m_position.y = y;
    m_position.o = o;
}

void WorldObject::ReLocate(const float &x, const float &y)
{
    m_position.x = x;
    m_position.y = y;
}

void WorldObject::ReLocate(const Location &loc)
{
    m_mapID      = loc.map;
    m_position.x = loc.x;
    m_position.y = loc.y;
    m_position.o = loc.o;
}