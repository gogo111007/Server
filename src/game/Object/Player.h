#ifndef _SH_GAME_CLASS_PLAYER_H_
#define _SH_GAME_CLASS_PLAYER_H_

#include "Unit.h"

class WorldSession;

struct PlayerInfo
{
    std::string name;
    uint32      guid;
    uint32      id;
    uint8       level;
    uint8       race;
    uint8       classs;
    uint32      health;
    uint32      mana;
    float       speed;
};

class Player : public Unit
{
    public:
        Player(WorldSession *pSession);
        virtual ~Player() { }

        virtual void  Update(const uint32 &diff);

        WorldSession *getSession() const { return m_pSession; }

    private:
        WorldSession    *m_pSession;
};

#endif