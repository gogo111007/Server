#ifndef _SH_GAME_CLASS_UNIT_H_
#define _SH_GAME_CLASS_UNIT_H_

#include "WorldObject.h"

class Unit : public WorldObject
{
    public:
        virtual ~Unit() { }

        virtual void Create(std::string name, uint32 guid, uint32 id, uint8 level, uint8 race, uint8 classs, uint32 health, uint32 mana, float speed);

        virtual void Update(const uint32 &diff);

        uint32   getID()        const  { return m_id;      }
        uint8    getLevel()     const  { return m_level;   }
        uint8    getRace()      const  { return m_race;    }
        uint8    getClass()     const  { return m_class;   }
        uint32   getHealth()    const  { return m_health;  }
        uint32   getMana()      const  { return m_mana;    }
        float    getSpeed()     const  { return m_speed;   }

    private:
        uint32   m_id;
        uint8    m_level;
        uint8    m_race;
        uint8    m_class;
        uint32   m_health;
        uint32   m_mana;
        float    m_speed;

    protected:
        explicit Unit():WorldObject(), m_id(0), m_level(0), m_race(0), m_class(0), m_health(0), m_mana(0), m_speed(1.0f) { }
};

#endif