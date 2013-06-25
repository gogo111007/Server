#include "Object.h"

void Object::AddToWorld()
{
    if (m_isInWorld)
        return;

    m_isInWorld = true;
}

void Object::RemoveFromWorld()
{
    if (!m_isInWorld)
        return;

    m_isInWorld = false;
}