#pragma once
#include <d3d9.h>

class EmptyScene
{
public:
    EmptyScene(){}
    virtual ~EmptyScene(){}

    virtual void onCreateDevice(IDirect3DDevice9* pd3dDevice){}
    virtual void onDestroyDevice(){}

    virtual void onLostDevice(){}
    virtual void onResetDevice(IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc){}
    //
    virtual void render(IDirect3DDevice9* pd3dDevice){}
    virtual void frameMove(float deltaTime){}
    virtual void handleMsg(HWND hWnd, UINT msg, WPARAM, LPARAM){}

    virtual void setDragging(bool isDragging){}
};
