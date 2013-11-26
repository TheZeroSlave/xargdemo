#include "DXUT.h"
#include "utils.h"

namespace
{

    struct SPRITE_VERTEX
    {
        Vec4 position;
        DWORD color;
        Vec2 tex1;
    };
}

Texture* LoadTextureFromFile(Device* device, const char* filename)
{
    Texture* tmp = 0;

    D3DXIMAGE_INFO imageInfo;
    D3DXGetImageInfoFromFileA(filename, &imageInfo);
    //load with 
    D3DXCreateTextureFromFileExA(device, filename, imageInfo.Width, imageInfo.Height, 
        D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, &imageInfo,
        0, &tmp);
    return tmp;
}

Effect*  LoadEffectFromFile(Device* device, const char* filename)
{
    DXBuffer* errorBuffer = 0;
    Effect* effect = 0;

    D3DXCreateEffectFromFileA(device, filename, 0, 0, 0, 0, &effect, &errorBuffer);
    if (errorBuffer)
    {
        const char* errorStr = (const char*)errorBuffer->GetBufferPointer();
        MessageBoxA(0, errorStr, "Error in shader", MB_ICONSTOP);
        return 0;
    }
    return effect;
}

void DrawScreenSprite(Device* device, Texture* texture, const Vec2& pos, const Vec2& sz, DWORD color)
{
    Vec4 _pos(pos.x - 0.5f, pos.y - 0.5f, 1, 1);
    SPRITE_VERTEX vertexes[] =
    {
        { _pos, color, Vec2(0, 0) },
        { _pos + Vec4(sz.x, 0, 0, 0), color, Vec2(1, 0) },
        { _pos + Vec4(0, sz.y, 0, 0), color, Vec2(0, 1) },
        { _pos + Vec4(sz.x, sz.y, 0, 0), color, Vec2(1, 1) }
    };
    device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);

    if (texture)
    {
        device->SetTexture(0, texture);
    }
    
    device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertexes, sizeof(SPRITE_VERTEX)); 

    //device->SetTexture(0, 0);
}

void DrawWorldSprite(Device* device, Texture* texture, const Vec3& pos, const Vec3& sz, DWORD color)
{
    SPRITE_VERTEX_WORLD vertexes[] =
    {
        { pos, color, Vec2(0, 0) },
        { pos + Vec3(sz.x, 0, 0), color, Vec2(1, 0) },
        { pos + Vec3(0, sz.y, 0), color, Vec2(0, 1) },
        { pos + sz, color, Vec2(1, 1) }
    };
    device->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1);

    if (texture)
    {
        device->SetTexture(0, texture);
    }
    
    device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertexes, sizeof(SPRITE_VERTEX_WORLD)); 

    //device->SetTexture(0, 0);
}