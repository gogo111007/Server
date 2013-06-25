#ifndef _SH_GAME_CLASS_OBJECTMANAGER_H_
#define _SH_GAME_CLASS_OBJECTMANAGER_H_

#include <common.h>
#include <SHFramework.h>

class Object;

NAMESPACE_SH_USE
NAMESPACE_STD_USE

class ObjectManager
{
    public:
        ObjectManager() { }
        ~ObjectManager() { }

    private:
};

#define sObjectManager SHSingleton<ObjectManager>::instance()

#endif