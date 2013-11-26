#pragma once
#include "types.h"

class Scene
{
public:
    Scene(){}
    virtual ~Scene(){}

    virtual void onCreateDevice(Device* device){}
    virtual void onDestroyDevice(){}

    virtual void onLostDevice(){}
    virtual void onResetDevice(IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc){}
    //
    virtual void render(IDirect3DDevice9* pd3dDevice){}
    virtual void frameMove(float deltaTime){}
    virtual void handleMsg(HWND hWnd, UINT msg, WPARAM, LPARAM){}

    virtual void setDragging(bool isDragging){}

    virtual void drawSprite(Texture* texture, const Vec2& pos, const Vec2& sz, DWORD color = 0xFFFFFFFF){}
    virtual Vec2 size() const = 0;
};
