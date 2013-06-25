#include "Game.h"
#include "SocketEventHandler.h"
#include "World.h"
#include "TimeLine.h"

NAMESPACE_STD_USE

void Game::Init()
{
    if (m_initialized)
        return;

    try
    {
        InitNetwork();
        InitMsgCodes();

        sWorld.Init();
        sTimeLine.Init();

        m_initialized = true;
    }
    catch (SHException e)
    {
        cout << e.what() << endl;
        exit(1);
    }
}

void Game::Release()
{
    SAFE_RELEASE_DEL(m_pNetwork);
}

void Game::Go()
{
    if (!m_initialized)
        Init();

    sTimeLine.Start();
}

void Game::InitNetwork()
{
    m_pNetwork = new SHNetwork(SERVER, TCP, IPv4, "", 9897);

    m_pNetwork->Init();

    m_pNetwork->getGroup()->setHandler(new SocketEventHandler());

    cout << "Init Network succeed, server listening on port: " << m_pNetwork->getSocket()->localPort() << endl;
}

void Game::Update(uint32 diff)
{
    m_pNetwork->getGroup()->Listen(0);
}
