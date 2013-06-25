#include "MsgCode.h"
#include "World.h"
#include "WorldSocket.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "Player.h"
#include "MapManager.h"

NAMESPACE_STD_USE

PlayerInfo playerInfo[] = {{"果果", 100, 0, 99, 1, 2, 82623, 3721, 1.0f}, {"李四", 101, 1, 5, 3, 3, 10371, 3721, 1.0f},
                           {"王二", 102, 2, 50, 4, 1, 39823, 3721, 1.0f}, {"小王", 103, 3, 6, 5, 5, 10432, 3721, 1.0f},
                           {"二货", 104, 4, 32, 6, 1, 16234, 3721, 1.0f}, {"傻缺", 105, 5, 3, 2, 3, 10156, 3721, 1.0f},
                           {"牛逼", 106, 6, 18, 1, 6, 12389, 3721, 1.0f}, {"老子", 107, 7, 1, 1, 7, 10347, 3721, 1.0f},
                           {"孔子", 108, 8, 21, 2, 4, 17389, 3721, 1.0f}, {"仔仔", 109, 9, 9, 5, 8, 11679, 3721, 1.0f}};

WorldSession::WorldSession(WorldSocket *pSocket, uint32 accountID):
    m_pSocket(pSocket), m_accountID(accountID), m_clientType(CLIENT_TYPE_UNKNOW),
    m_pPlayer(nullptr), m_inQueue(false)
{
}

WorldSession::~WorldSession()
{
    if (m_pPlayer)
        LogOutPlayer();

    if (m_pSocket)
        m_pSocket->Close();
}

int32 WorldSession::Update(const uint32 &diff)
{
    if (!m_pSocket || m_pSocket->closed())
        return -1;

    return 1;
}

void WorldSession::KickPlayer()
{
    if (m_pSocket)
        m_pSocket->Close();
}

void WorldSession::LogOutPlayer()
{
    WorldPacket data(SMSG_PLAYER_LOGOUT);

    data << m_pPlayer->getID();

    sWorld.SendGlobalWorldPacket(data);

    m_pPlayer->getMap()->Remove(m_pPlayer);

    SAFE_DELETE(m_pPlayer);
}

void WorldSession::HandleNULL(WorldPacket &recvPacket)
{
    cout << "ERROR: Handle NULL MsgCode " << recvPacket.getMsgCode() << "(" << LookupMsgCodeName(recvPacket.getMsgCode()) << ")" << endl;
}

void WorldSession::HandleEarlyProccess(WorldPacket &recvPacket)
{
    cout << "ERROR: Handle EarlyProccess MsgCode " << recvPacket.getMsgCode() << "(" << LookupMsgCodeName(recvPacket.getMsgCode()) << ")" << endl;
}

void WorldSession::HandleServerSide(WorldPacket &recvPacket)
{
    cout << "ERROR: Handler ServerSide MsgCode " << recvPacket.getMsgCode() << "(" << LookupMsgCodeName(recvPacket.getMsgCode()) << ")" << endl;
}

void WorldSession::HandleTest(WorldPacket &recvPacket)
{
    cout << "Handle " << LookupMsgCodeName(recvPacket.getMsgCode()) << ", size: " << recvPacket.Size() << endl;
}

void WorldSession::HandleEnumCharacters(WorldPacket &recvPacket)
{
    cout << "Handle " << LookupMsgCodeName(recvPacket.getMsgCode()) << ", size: " << recvPacket.Size() << endl;

    WorldPacket data(SMSG_ENUM_CHARACTERS, sizeof(PlayerInfo));

    data << playerInfo[m_accountID].name
         << playerInfo[m_accountID].guid
         << playerInfo[m_accountID].id
         << playerInfo[m_accountID].level
         << playerInfo[m_accountID].race
         << playerInfo[m_accountID].classs
         << playerInfo[m_accountID].health
         << playerInfo[m_accountID].mana
         << playerInfo[m_accountID].speed;

    SendPacket(&data);
}

void WorldSession::HandlePlayerInfo(WorldPacket &recvPacket)
{
    cout << "Handle " << LookupMsgCodeName(recvPacket.getMsgCode()) << ", size: " << recvPacket.Size() << endl;

    uint32 id;
    recvPacket >> id;

    if (id < 10)
    {
        WorldPacket data(SMSG_PLAYER_INFO, sizeof(PlayerInfo));

        data << playerInfo[id].name
             << playerInfo[id].guid
             << playerInfo[id].id
             << playerInfo[id].level
             << playerInfo[id].race
             << playerInfo[id].classs
             << playerInfo[id].health
             << playerInfo[id].mana
             << playerInfo[id].speed;

        SendPacket(&data);
    }
}

void WorldSession::HandlePlayerLogin(WorldPacket &recvPacket)
{
    cout << "Handle " << LookupMsgCodeName(recvPacket.getMsgCode()) << ", size: " << recvPacket.Size() << endl;

    uint32 id = recvPacket.ReadUInt32();

    Player *pPlayer = new Player(this);
    pPlayer->Create(playerInfo[m_accountID].name, playerInfo[m_accountID].guid, playerInfo[m_accountID].id, playerInfo[m_accountID].level, playerInfo[m_accountID].race,
        playerInfo[m_accountID].classs, playerInfo[m_accountID].health, playerInfo[m_accountID].mana, playerInfo[m_accountID].speed);

    pPlayer->setPosition(id * 103.5f, id * 25.3f, 0);

    setPlayer(pPlayer);

    WorldPacket data(SMSG_HELLO);

    string msg = "欢迎登陆蘑菇王子服务器！@~~";

    data << m_accountID;
    data << sWorld.GetActivedSessionCount();
    data << msg;

    SendPacket(&data);

    sMapManager.LoadMap(pPlayer->getMapID())->Add(pPlayer);

    msg = "有玩家登陆啦~！@@@";

    data.Initialize(SMSG_PLAYER_LOGIN, sizeof(uint32) + msg.size() + m_pPlayer->getName().size());

    data << id;
    data << m_pPlayer->getName();
    data << msg;

    sWorld.SendGlobalWorldPacket(data);
}

void WorldSession::HandlePlayerLogOut(WorldPacket &recvPacket)
{
    cout << "Handle " << LookupMsgCodeName(recvPacket.getMsgCode()) << ", size: " << recvPacket.Size() << endl;
}

void WorldSession::HandleChatMessage(WorldPacket &recvPacket)
{
    cout << "Handle " << LookupMsgCodeName(recvPacket.getMsgCode()) << ", size: " << recvPacket.Size() << endl;

    string s = recvPacket.ReadString();

    sWorld.SendWorldMessage(s, m_pPlayer->getID());
}

void WorldSession::HandleMoveStartForward(WorldPacket &recvPacket)
{
    cout << "Handle " << LookupMsgCodeName(recvPacket.getMsgCode()) << ", size: " << recvPacket.Size() << endl;

    uint32 id = recvPacket.ReadUInt32();

    WorldPacket data(SMSG_PLAYER_MOVE);

    data << id;

    sWorld.SendGlobalWorldPacket(data);
}

void WorldSession::HandleMoveStartBackward(WorldPacket &recvPacket)
{
    cout << "Handle " << LookupMsgCodeName(recvPacket.getMsgCode()) << ", size: " << recvPacket.Size() << endl;

    uint32 id = recvPacket.ReadUInt32();

    WorldPacket data(SMSG_PLAYER_MOVE, 4);

    data << id;

    sWorld.SendGlobalWorldPacket(data);
}

void WorldSession::HandleMoveStop(WorldPacket &recvPacket)
{
    cout << "Handle " << LookupMsgCodeName(recvPacket.getMsgCode()) << ", size: " << recvPacket.Size() << endl;

    uint32 id = recvPacket.ReadUInt32();

    WorldPacket data(SMSG_PLAYER_MOVE, 4);

    data << id;

    sWorld.SendGlobalWorldPacket(data);
}

void WorldSession::HandleMoveJump(WorldPacket &recvPacket)
{
    cout << "Handle " << LookupMsgCodeName(recvPacket.getMsgCode()) << ", size: " << recvPacket.Size() << endl;
}

void WorldSession::HandleMovementInfo(WorldPacket &recvPacket)
{
    cout << "Handle " << LookupMsgCodeName(recvPacket.getMsgCode()) << ", size: " << recvPacket.Size() << endl;

    uint32 id    = recvPacket.ReadUInt32();

    WorldPacket data(SMSG_MOVEMENT_INFO, 4);

    data << id;

    sWorld.SendGlobalWorldPacket(data);
}

void WorldSession::HandleMoveStateChanged(WorldPacket &recvPacket)
{
    cout << "Handle " << LookupMsgCodeName(recvPacket.getMsgCode()) << ", size: " << recvPacket.Size() << endl;

    uint32 id = recvPacket.ReadUInt32();

    WorldPacket data(SMSG_MOVESTATE_CHANGED, 4);

    data << id;

    sWorld.SendGlobalWorldPacket(data);
}

int32 WorldSession::SendPacket(WorldPacket const *sendPacket)
{
    if (!m_pSocket || m_pSocket->closed())
        return -1;

    if (sendPacket->getMsgCode() == SMSG_CHATMESSAGE && !m_pPlayer)
        return 0;

    //cout << "Send To " << m_pSocket->getRemoteHost() << ", MsgCode: " << sendPacket->getMsgCode() << ", Size: " << sendPacket->Size() << endl;

    return m_pSocket->SendPacket(*sendPacket);
}