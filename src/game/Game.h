#ifndef _SH_GAME_CLASS_GAME_H_
#define _SH_GAME_CLASS_GAME_H_

#include <common.h>
#include <SHFramework.h>

NAMESPACE_SH_USE
NAMESPACE_STD_USE

class Game
{
    public:
        Game(): m_pNetwork(NULL), m_initialized(false) { }
        ~Game() { }

        void Init();
        void Update(uint32 diff);
        void Release();
        void Go();

    private:
        void   InitNetwork();

        bool   m_initialized;

        SHNetwork *m_pNetwork;
};

#define sGame SHSingleton<Game>::instance()

#endif

