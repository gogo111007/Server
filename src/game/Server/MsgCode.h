#ifndef _SH_GAME_MSGCODE_H_
#define _SH_GAME_MSGCODE_H_

#include <common.h>

#include "WorldSession.h"

enum MsgCodes
{
    MSG_CONNECTION                                       = 0x0000,
    MSG_PING                                             = 0x0001,
    SMSG_AUTH_CHALLENGE                                  = 0x0002,
    CMSG_AUTH_SESSION                                    = 0x0003,
    SMSG_AUTH_RESPONSE                                   = 0x0004,
    SMSG_HELLO                                           = 0x0005,
    SMSG_ENUM_CHARACTERS                                 = 0x0006,
    CMSG_ENUM_CHARACTERS                                 = 0x0007,
    SMSG_PLAYER_INFO                                     = 0x0008,
    CMSG_PLAYER_INFO                                     = 0x0009,
    SMSG_TEST                                            = 0x0C00,
    CMSG_TEST                                            = 0x0C01,
    SMSG_PLAYER_LOGIN                                    = 0x3DF0,
    CMSG_PLAYER_LOGIN                                    = 0x3DF1,
    SMSG_PLAYER_LOGOUT                                   = 0x3DF2,
    CMSG_PLAYER_LOGOUT                                   = 0x3DF3,
    SMSG_CHATMESSAGE                                     = 0x3DF8,
    CMSG_CHATMESSAGE_SAY                                 = 0x3DF9,
    SMSG_PLAYER_MOVE                                     = 0x4D00,
    CMSG_MOVE_START_FORWARD                              = 0x4D01,
    CMSG_MOVE_START_BACKWARD                             = 0x4D02,
    CMSG_MOVE_STOP                                       = 0x4D05,
    CMSG_MOVE_JUMP                                       = 0x4D06,
    SMSG_MOVEMENT_INFO                                   = 0x4D07,
    CMSG_MOVEMENT_INFO                                   = 0x4D08,
    SMSG_MOVESTATE_CHANGED                               = 0x4D09,
    CMSG_MOVESTATE_CHANGED                               = 0x4D10,
};

#define MAX_MSGCODE_TABLE_SIZE 0xFFFF

extern void InitMsgCodes();

enum SessionStatus
{
    STATUS_NULL,
};

struct MsgCodeHandler
{
    const char    *pName;
    SessionStatus  status;
    void (WorldSession::*handler)(WorldPacket &recvPacket);
};

extern MsgCodeHandler msgCodeTable[MAX_MSGCODE_TABLE_SIZE];

inline const char *LookupMsgCodeName(uint32 id)
{
    return msgCodeTable[id].pName;
}

#endif
