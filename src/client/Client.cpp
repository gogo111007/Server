#include "Client.h"

NAMESPACE_SH_USE
NAMESPACE_STD_USE

void InputChatMsg()
{
    try
    {
        while (!disconnect)
        {
            Sleep(500);

            if (!loggedIn || !authed)
                continue;

            GetInputPos(&inputPosX, &inputPosY);

            char msg[255];

            cin.getline(msg, 255);

            if(!strcmp(msg, "exit"))
                disconnect = true;
            else if (strcmp(msg, "") && strcmp(msg, "\n"))
            {
                WorldPacket data(CMSG_CHATMESSAGE_SAY, sizeof(msg));
                data << msg;
                SendPacket(data);
            }
        }
    }
    catch (SHException e)
    {
        cout << e.what() << endl;
    }
}

void ParsePacket()
{
    try
    {
        bool locked = false;
        while (!disconnect)
        {
            Sleep(1);

            if (locked || packetQueueIn.empty())
                continue;
            else
                locked = true;

            WorldPacket data = packetQueueIn.front();
            packetItr itr = packetQueueIn.begin();
            packetQueueIn.erase(itr);

            uint32 msgCode = data.getMsgCode();

            switch (msgCode)
            {
                case MSG_CONNECTION:
                {
                    string msg;
                    data >> msg;

                    cout << "[System]: Chanllenge OK: " << msg << endl;
                    challengeOK = true;
                    break;
                }
                case SMSG_AUTH_CHALLENGE:
                {
                    if (challengeOK)
                    {
                        WorldPacket data(CMSG_AUTH_SESSION);
                        data << username;
                        data << password;

                        SendPacket(data);
                    }
                    else
                        cout << "[System]: Client not prepared!" << endl;
                    break;
                }
                case SMSG_AUTH_RESPONSE:
                {
                    string msg;

                    data >> authed;
                    data >> msg;

                    cout << "[System]: " << msg << endl;

                    WorldPacket sData;

                    if (!authed)
                    {
                        cin >> username >> password;

                        sData.Initialize(CMSG_AUTH_SESSION);
                        sData << username;
                        sData << password;

                        SendPacket(sData);
                    }
                    else
                    {
                        sData.Initialize(CMSG_ENUM_CHARACTERS);
                        sData << uint8(0);
                        SendPacket(sData);
                    }
                    break;
                }
                case SMSG_HELLO:
                {
                    uint32 onLinePlayer;
                    string msg;

                    data >> accountID;
                    data >> onLinePlayer;
                    data >> msg;

                    cout << "[System]: " << "AccountID: " << accountID << ", Name: " << player.name << ", Online Players: " << onLinePlayer << "\n[System]: " << msg << endl;

                    break;
                }
                case SMSG_ENUM_CHARACTERS:
                {
                    data >> player.name
                         >> player.guid
                         >> player.id
                         >> player.level
                         >> player.race
                         >> player.classs
                         >> player.health
                         >> player.mana
                         >> player.speed;

                    uint32 id;
                    while (true)
                    {
                        cout << "[System]: Choose your character: " << player.id << endl;
                        cin >> id;

                        if (id == player.id)
                            break;

                        cout << "[System]: You don't have that character!" << endl;
                    }

                    playerMap[player.id] = player.name;

                    loggedIn = true;

                    WorldPacket sData(CMSG_PLAYER_LOGIN);
                    sData << player.id;

                    SendPacket(sData);

                    break;
                }
                case SMSG_PLAYER_LOGIN:
                {
                    uint32 id;
                    string name;
                    string msg;

                    data >> id;
                    data >> name;
                    data >> msg;

                    if (id != accountID)
                    {
                        cout << "[System]: " << msg << ", ID: " << id << ", Name: " << name << endl;
                        playerMap[id] = name;
                    }
                    break;
                }
                case SMSG_PLAYER_INFO:
                {
                    PlayerInfo *info = new PlayerInfo();

                    data >> info->name
                         >> info->guid
                         >> info->id
                         >> info->level
                         >> info->race
                         >> info->classs
                         >> info->health
                         >> info->mana
                         >> info->speed;

                    cout << "Player:[Name:" << info->name << ", GUID:" << info->guid << ", ID:" << info->id << ", LV:" << uint32(info->level)
                         << ", R:" << uint32(info->race) << ", C:" << uint32(info->classs) << ", HP:" << info->health << ", MP:" << info->mana << ", S:" << info->speed << "]" << endl;

                    if (playerMap[info->id] == "")
                        playerMap[info->id] = info->name;

                    SAFE_DELETE(info);

                    break;
                }
                case SMSG_CHATMESSAGE:
                {
                    int32  sender;
                    string msg;
                    data >> sender;
                    data >> msg;

                    if (sender < 0)
                        cout << "[System]: " << msg << endl;
                    else
                    {
                        if (playerMap[sender] == "")
                        {
                            WorldPacket sData(CMSG_PLAYER_INFO);
                            sData << sender;
                            SendPacket(sData);
                            break;
                        }

                        if (sender == player.id)
                            SetInputPos(inputPosX, inputPosY);

                        cout << "[" << playerMap[sender] << "]: " << msg << endl;
                    }
                    break;
                }
                case SMSG_PLAYER_LOGOUT:
                {
                    uint32 id = data.ReadUInt32();
                    if (playerMap[id] != "")
                        cout << "[System]: Player [" << playerMap[id] << "] Logged out!" << endl;

                    break;
                }
                default:
                    cout << "[System]: Get UnHandled MsgCode: " << msgCode << endl;
            }

            GetInputPos(&inputPosX, &inputPosY);
            locked = false;
        }
    }
    catch (SHException e)
    {
        cout << e.what() << endl;
    }
}

int32 main()
{
    cout << "Please input your username and password:\n";

    cin >> username >> password;

    try
    {
        if (!network.Init())
        {
            cout << "Init Network failed!\n";
            system("pause");
            return 1;
        }
        else
        {
            network.getGroup()->setHandler(new SocketEventHandler());

            thread t1(Listen);
            thread t2(InputChatMsg);
            thread t3(ParsePacket);

            t1.join();
            t2.join();
            t3.join();
        }
    }
    catch (SHException e)
    {
        cout << "***ERROR***\n" << e.what() << endl;
    }
}
