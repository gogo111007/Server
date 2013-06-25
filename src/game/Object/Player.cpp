#include "Player.h"
#include "WorldSession.h"

Player::Player(WorldSession *pSession):
    Unit(), m_pSession(pSession)
{
    m_typeID = TYPEID_PLAYER;
}

void Player::Update(const uint32 &diff)
{
    Unit::Update(diff);
}