#include "Unit.h"

void Unit::Create(std::string name, uint32 guid, uint32 id, uint8 level, uint8 race, uint8 classs, uint32 health, uint32 mana, float speed)
{
    WorldObject::Create(name, guid);

    m_id      = id;
    m_level   = level;
    m_race    = race;
    m_class   = classs;
    m_health  = health;
    m_mana    = mana;
    m_speed   = speed;
}

void Unit::Update(const uint32 &diff)
{
    WorldObject::Update(diff);
}