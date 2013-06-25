#ifndef _SH_GAME_CLASS_GRID_H_
#define _SH_GAME_CLASS_GRID_H_

#include <SHFramework.h>

#define GRID_WIDTH     64
#define GRID_HEIGHT    64

class Object;
class Player;

template<typename KEY, class T>
class ObjectContainer : public SH::SHTypeContainer<KEY, T>
{

};

class Grid
{
    public:
        explicit Grid(uint32 locX = 0, uint32 locY = 0): m_locX(locX), m_locY(locY), m_posX(float(locX * GRID_WIDTH + GRID_WIDTH / 2.0f)), m_posY(float(locX * GRID_HEIGHT + GRID_HEIGHT / 2.0f)), m_unloadTimer(0) { }
        ~Grid() { m_objects.clear(); }

        void Update(uint32 diff) { if (!m_objects.size()) m_unloadTimer += diff; }

        void Add(Player *pPlayer);
        void Remove(Player *pPlayer);
        
        template<typename T>
        void Add(T obj);
        template<typename T>
        void Remove(T obj);

        uint32    getObjectCount() const { return m_objects.size(); }
        uint32    getUnloadTimer() const { return m_unloadTimer; }

        uint32    getLocX()   const  { return m_locX; }
        uint32    getLocY()   const  { return m_locY; }
        float     getPosX()   const  { return m_posX; }
        float     getPosY()   const  { return m_posY; }

        void      GetLocation(uint32 &x, uint32 &y) const { x = m_locX; y = m_locY; }
        void      GetPosition(float  &x, float  &y) const { x = m_posX; y = m_posY; }

        static void CalculateGridLocation(const float &posX, const float &posY, uint32 &locX, uint32 &locY)
        {
            locX = uint32(posX / GRID_WIDTH);
            locY = uint32(posY / GRID_HEIGHT);
        }

    private:
        ObjectContainer<uint32, Object*>  m_objects;

        uint32  m_unloadTimer;

        uint32  m_locX;
        uint32  m_locY;
        float   m_posX;
        float   m_posY;
};

#endif