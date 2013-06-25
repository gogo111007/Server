#include "MsgCode.h"

static void DefineMsgCode(uint32 msgCode, const char *pName, SessionStatus status, void (WorldSession::*handler)(WorldPacket &recvPacket))
{
    msgCodeTable[msgCode].pName   = pName;
    msgCodeTable[msgCode].status  = status;
    msgCodeTable[msgCode].handler = handler;
}

#define MSGCODE(name, status, handler) DefineMsgCode(name, #name, status, handler)

MsgCodeHandler msgCodeTable[MAX_MSGCODE_TABLE_SIZE];

void InitMsgCodes()
{
    for(uint32 i = 0; i < MAX_MSGCODE_TABLE_SIZE; ++i)
        DefineMsgCode(i, "UNKNOWN", STATUS_NULL, &WorldSession::HandleNULL);

    MSGCODE(MSG_CONNECTION,                        STATUS_NULL,          &WorldSession::HandleEarlyProccess            );
    MSGCODE(MSG_PING,                              STATUS_NULL,          &WorldSession::HandleEarlyProccess            );
    MSGCODE(SMSG_AUTH_CHALLENGE,                   STATUS_NULL,          &WorldSession::HandleServerSide               );
    MSGCODE(CMSG_AUTH_SESSION,                     STATUS_NULL,          &WorldSession::HandleEarlyProccess            );
    MSGCODE(SMSG_AUTH_RESPONSE,                    STATUS_NULL,          &WorldSession::HandleServerSide               );
    MSGCODE(SMSG_HELLO,                            STATUS_NULL,          &WorldSession::HandleServerSide               );
    MSGCODE(SMSG_ENUM_CHARACTERS,                  STATUS_NULL,          &WorldSession::HandleServerSide               );
    MSGCODE(CMSG_ENUM_CHARACTERS,                  STATUS_NULL,          &WorldSession::HandleEnumCharacters           );
    MSGCODE(SMSG_PLAYER_INFO,                      STATUS_NULL,          &WorldSession::HandleServerSide               );
    MSGCODE(CMSG_PLAYER_INFO,                      STATUS_NULL,          &WorldSession::HandlePlayerInfo               );
    MSGCODE(SMSG_TEST,                             STATUS_NULL,          &WorldSession::HandleServerSide               );
    MSGCODE(CMSG_TEST,                             STATUS_NULL,          &WorldSession::HandleEarlyProccess            );
    MSGCODE(SMSG_PLAYER_LOGIN,                     STATUS_NULL,          &WorldSession::HandleServerSide               );
    MSGCODE(CMSG_PLAYER_LOGIN,                     STATUS_NULL,          &WorldSession::HandlePlayerLogin              );
    MSGCODE(SMSG_PLAYER_LOGOUT,                    STATUS_NULL,          &WorldSession::HandleServerSide               );
    MSGCODE(CMSG_PLAYER_LOGOUT,                    STATUS_NULL,          &WorldSession::HandlePlayerLogOut             );
    MSGCODE(SMSG_CHATMESSAGE,                      STATUS_NULL,          &WorldSession::HandleServerSide               );
    MSGCODE(CMSG_CHATMESSAGE_SAY,                  STATUS_NULL,          &WorldSession::HandleChatMessage              );
    MSGCODE(SMSG_PLAYER_MOVE,                      STATUS_NULL,          &WorldSession::HandleServerSide               );
    MSGCODE(CMSG_MOVE_START_FORWARD,               STATUS_NULL,          &WorldSession::HandleMoveStartForward         );
    MSGCODE(CMSG_MOVE_START_BACKWARD,              STATUS_NULL,          &WorldSession::HandleMoveStartBackward        );
    MSGCODE(CMSG_MOVE_STOP,                        STATUS_NULL,          &WorldSession::HandleMoveStop                 );
    MSGCODE(CMSG_MOVE_JUMP,                        STATUS_NULL,          &WorldSession::HandleMoveJump                 );
    MSGCODE(SMSG_MOVEMENT_INFO,                    STATUS_NULL,          &WorldSession::HandleServerSide               );
    MSGCODE(CMSG_MOVEMENT_INFO,                    STATUS_NULL,          &WorldSession::HandleMovementInfo             );
    MSGCODE(SMSG_MOVESTATE_CHANGED,                STATUS_NULL,          &WorldSession::HandleServerSide               );
    MSGCODE(CMSG_MOVESTATE_CHANGED,                STATUS_NULL,          &WorldSession::HandleMoveStateChanged         );
};