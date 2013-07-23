#include "engine\EmptyScene.h"

class DemoScene
{
public:
    DemoScene();
    ~DemoScene();

    void onCreateDevice(IDirect3DDevice9* pd3dDevice);
    void onDestroyDevice();

    void onLostDevice();
    void onResetDevice(IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc);
    //
    void render(IDirect3DDevice9* pd3dDevice);
    void frameMove(float deltaTime);
    void handleMsg(HWND hWnd, UINT msg, WPARAM, LPARAM);

    void setDragging(bool isDragging);

private:
    bool m_isDragged;
};