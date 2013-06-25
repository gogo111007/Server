#include <Network\SHNetwork.h>
#include <thread>

#include "MsgCode.h"
#include "WorldPacket.h"

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
} player;

map<uint32, string> playerMap;

const string SERVER_HOST   = "localhost";
const uint32 SERVER_PORT   = 9897;

uint32  accountID   = 0;
string  username    = "";
string  password    = "";
int32   inputPosX   = 0;
int32   inputPosY   = 0;

bool    authed      = false;
bool    loggedIn    = false;
bool    challengeOK = false;

volatile bool disconnect  = false;

std::vector<WorldPacket> packetQueueIn;
typedef std::vector<WorldPacket>::const_iterator packetItr;

SHNetwork network(CLIENT, TCP, ANY, SERVER_HOST, 0, SERVER_PORT);

void SetInputPos(int32 x, int32 y)
{
    COORD   pos    = {x, y};
    HANDLE  handle = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(handle, pos);
}

void GetInputPos(int32 *x, int32 *y)
{
    CONSOLE_SCREEN_BUFFER_INFO console;
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo(handle, &console);
    *x =console.dwCursorPosition.X;
    *y =console.dwCursorPosition.Y;
}

void SendPacket(WorldPacket &data)
{
    if (disconnect)
        return;

    size_t len   = data.Size() + 4;
    byte *buffer = new byte[len];

    PacketHeader header(data.Size(), data.getMsgCode());

    memset(buffer, 0, len);
    memcpy(buffer, header.header(), header.size());
    memcpy(buffer + header.size(), data.Contents(), data.Size());

    network.getSocket()->Send(buffer, len);

    delete [] buffer;
    buffer = NULL;
}

void Listen()
{
    try
    {
        while (!disconnect)
            network.getGroup()->Listen(500);
    }
    catch (SHException e)
    {
    }
}

class SocketEventHandler: public SHSocketEventHandler
{
    void OnAccept(SHSocket *pSocket, SHSocketGroup *pGroup, void *pReference)
    {
    }

    void OnRead(SHSocket *pSocket, SHSocketGroup *pGroup, void *pReference)
    {
        if (!pSocket || pSocket->nextReadSize() <= 0)
            return;

        uint32 readSize = pSocket->nextReadSize();
        byte *buffer = new byte[readSize];
        pSocket->Read(buffer, readSize);

        uint32 size    = 0;
        uint16 msgCode = 0;
        uint32 readPos = 0;

        while (readPos != readSize)
        {
            size     = ((buffer[readPos + 0] & 0xFF) << 8) | (buffer[readPos + 1] & 0xFF);
            msgCode  = ((buffer[readPos + 2] & 0xFF) << 8) | (buffer[readPos + 3] & 0xFF);
            readPos += 4;

            WorldPacket data = WorldPacket(msgCode, size);
            data.Append(&buffer[readPos], size);
            readPos += size;

            packetQueueIn.push_back(data);
        }

        delete [] buffer;
    }

    void OnDisconnect(SHSocket *pSocket, SHSocketGroup *pGroup, void *pReference)
    {
        disconnect = true;
        pGroup->Remove(pSocket);

        cout << "Disconnect from server..." << endl;
    }
};