#pragma once
#include "types.h"

Texture* LoadTextureFromFile(Device* device, const char* filename);
Effect*  LoadEffectFromFile(Device* device, const char* filename);

//draws sprite in scene coordinates
void DrawScreenSprite(Device* device, Texture* texture, const Vec2& pos, const Vec2& sz, DWORD color = 0xFFFFFFFF);
void DrawWorldSprite(Device* device, Texture* texture, const Vec3& pos, const Vec3& sz, DWORD color = 0xFFFFFFFF);

#define FOREACH_STD(ContainerType, Container) for (ContainerType::iterator it = Container.begin(); it != Container.end(); ++it)

struct SPRITE_VERTEX_WORLD
{
    Vec3 position;
    DWORD color;
    Vec2 tex1;
};
#define SPRITE_VERTEX_FVF (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1)

namespace Math
{
    template<class T>
    T clamp(const T& value, const T& min, const T& max)
    {
        if (value > max)
        {
            return max;
        }

        if (value < min)
        {
            return min;
        }
        return value;
    }

    template<class T>
    T randomRange(const T& minV, const T maxV)
    {
        T range = maxV - minV;
        double r = rand() / (double)RAND_MAX;
        return minV + (T)(r * (range));
    }
}
