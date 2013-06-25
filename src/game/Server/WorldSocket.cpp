#include "World.h"
#include "WorldPacket.h"
#include "WorldSocket.h"
#include "WorldSession.h"

Account accountList[] = {{0, "chglove", "123"}, {1, "test1", "123"}, {2, "test2", "123"}, {3, "test3", "123"}, {4, "test4", "123"},
                         {5, "test5",   "123"}, {6, "test6", "123"}, {7, "test7", "123"}, {8, "test8", "123"}, {9, "test9", "123"}};

WorldSocket::WorldSocket(SHSocket *pSHSocket):
    m_pSHSocket(pSHSocket), m_pSession(NULL), m_pRecvPacket(NULL), m_pSendHeader(NULL), m_closed(true), m_missedDataSize(0)
{
}

WorldSocket::~WorldSocket()
{
    if (m_pSession)
        m_pSession->m_pSocket = NULL;

    m_recvDataHolder.clear();
    m_sendDataHolder.clear();

    SAFE_DELETE(m_pRecvPacket);
    SAFE_DELETE(m_pSendHeader);
}

void WorldSocket::Open()
{
    cout << "Accept connection from " << getRemoteHost() << ":" << getRemotePort() << endl;

    m_closed = false;

    m_pSendHeader = new PacketHeader(0, 0);

    string serverToClient = "CONNECTION SUCCESS - SERVER TO CLIENT";

    WorldPacket data(MSG_CONNECTION);
    data << serverToClient;

    SendPacket(data);

    data.Initialize(SMSG_AUTH_CHALLENGE, 4);
    data << 0;

    SendPacket(data);
}

void WorldSocket::Close()
{
    if (m_closed)
        return;

    m_closed = true;

    SAFE_CALL_VOID(m_pSHSocket, Disconnect());
}

void WorldSocket::HandleReceiveData()
{
    if (m_closed || !m_pSHSocket || m_pSHSocket->disconnected() || m_pSHSocket->nextReadSize() <= 0)
        return;

    try
    {
        uint16 readSize = m_pSHSocket->nextReadSize();
        uint16 readPos  = 0;
        m_recvDataHolder.resize(readSize);
        m_pSHSocket->Read(&m_recvDataHolder[0], readSize);

        cout << "Read From " << getRemoteHost() << ":" << getRemotePort() << ", Data Size: " << readSize << endl;

        while (readSize)
        {
            if (m_missedDataSize)
            {
                uint16 missedSize = m_missedDataSize >= readSize ? readSize : m_missedDataSize;
                m_pRecvPacket->Append(&m_recvDataHolder[0], missedSize);
                readSize         -= missedSize;
                m_missedDataSize -= missedSize;
                readPos          += missedSize;

                if (!m_missedDataSize)
                {
                    ProcessIncomingPacket(m_pRecvPacket);
                    m_pRecvPacket = NULL;
                }
            }
            else
            {
                if (readSize < 4)
                    throw SHException(SHException::SH_EXCEPTION_OUT_OF_RANGE, "Missed packet header info!");

                uint32 size    = ((m_recvDataHolder[readPos++] & 0xFF) << 8) | (m_recvDataHolder[readPos++] & 0xFF);
                uint16 msgCode = ((m_recvDataHolder[readPos++] & 0xFF) << 8) | (m_recvDataHolder[readPos++] & 0xFF);

                readSize -= 4;

                m_pRecvPacket = new WorldPacket(msgCode, size);

                if (size > readSize)
                {
                    m_missedDataSize  = size - readSize;
                    if (readSize)
                    {
                        m_pRecvPacket->Append(&m_recvDataHolder[readPos], readSize);
                        readPos  += readSize;
                        readSize -= readSize;
                    }
                }
                else
                {
                    m_pRecvPacket->Append(&m_recvDataHolder[readPos], size);
                    readPos  += size;
                    readSize -= size;

                    ProcessIncomingPacket(m_pRecvPacket);
                    m_pRecvPacket = NULL;
                }
            }
        }

        m_recvDataHolder.clear();
    }
    catch (SHException e)
    {
        m_recvDataHolder.clear();

        cout << e.what() << endl;
    }
}

void WorldSocket::ProcessIncomingPacket(WorldPacket *recvPacket)
{
    uint16 msgCode = recvPacket->getMsgCode();

    switch (msgCode)
    {
        case MSG_CONNECTION:
        {
            HandleConnection(*recvPacket);
            break;
        }
        case MSG_PING:
        {
            HandlePing(*recvPacket);
            break;
        }
        case CMSG_AUTH_SESSION:
        {
            if (m_pSession)
            {
                cout << "Client try to auth again!" << endl;
                return;
            }
            HandleAuthSession(*recvPacket);
            break;
        }
        default:
        {
            if (m_pSession)
            {
                const MsgCodeHandler msghandler = msgCodeTable[msgCode];

                (m_pSession->*msghandler.handler)(*recvPacket);
            }
            else
                cout << "ERROR: Client not authed! Remote Host: " << getRemoteHost() << ", MsgCode: "<< msgCode << "(" << LookupMsgCodeName(msgCode) << ")" << endl;
        }
    }

    SAFE_DELETE(recvPacket);
}

void WorldSocket::HandleConnection(WorldPacket &recvPacket)
{
    string clientToServer = "";

    recvPacket >> clientToServer;
}

void WorldSocket::HandlePing(WorldPacket &recvPacket)
{

}

void WorldSocket::HandleAuthSession(WorldPacket &recvPacket)
{
    string username = "";
    string password = "";

    recvPacket >> username;
    recvPacket >> password;

    WorldPacket data(SMSG_AUTH_RESPONSE);

    for each (Account account in accountList)
    {
        if (username == account.m_username && password == account.m_password)
        {
            string result = "Auth success, welcom to ShanHai Server!";
            data << bool(true);
            data << result;

            SendPacket(data);

            m_pSession = new WorldSession(this, account.m_accountID);
            //m_pSession->setPlayer(account.m_accountID);
            sWorld.AddSession(m_pSession);

            return;
        }
    }

    string result = "Bad username or password, please check your input and try again.";
    data << bool(false);
    data << result;

    SendPacket(data);
}

int32 WorldSocket::SendPacket(const WorldPacket &sendPacket)
{
    if (!m_pSHSocket || m_pSHSocket->disconnected())
        return -1;

    try
    {
        m_pSendHeader->Reset(sendPacket.Size(), sendPacket.getMsgCode());

        size_t len = m_pSendHeader->size() + sendPacket.Size();

        m_sendDataHolder.resize(len);

        memset(&m_sendDataHolder[0], 0, len);
        memcpy(&m_sendDataHolder[0], m_pSendHeader->header(), m_pSendHeader->size());
        memcpy(&m_sendDataHolder[m_pSendHeader->size()], sendPacket.Contents(), sendPacket.Size());

        m_pSHSocket->Send(&m_sendDataHolder[0], len);

        m_sendDataHolder.clear();

        return 1;
    }
    catch (SHException e)
    {
        cout << e.what() << endl;
        return 0;
    }
}
