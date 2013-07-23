#include <DXUT.h>
#include "demo.h"

DemoScene::DemoScene() : m_isDragged(false)
{
}

DemoScene::~DemoScene()
{
}

void DemoScene::onCreateDevice(IDirect3DDevice9* pd3dDevice)
{
}

void DemoScene::onDestroyDevice()
{
}

void DemoScene::onLostDevice()
{
}

void DemoScene::onResetDevice(IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc)
{
}

void DemoScene::render(IDirect3DDevice9* pd3dDevice)
{
}

void DemoScene::frameMove(float deltaTime)
{
}

void DemoScene::handleMsg(HWND hWnd, UINT msg, WPARAM, LPARAM)
{
}

void DemoScene::setDragging(bool isDragging)
{
    m_isDragged = isDragging;
}
