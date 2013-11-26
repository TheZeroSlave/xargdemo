#include <DXUT.h>
#include "demo.h"
#include "engine/Sphere.h"
#include <utility>
#include <string>

DemoScene::DemoScene() : m_isDragged(false)
{
    m_stars.reset(new Stars(0.0001f, m_engineFacade));
}

DemoScene::~DemoScene()
{
}

void DemoScene::onCreateDevice(IDirect3DDevice9* device)
{
    D3DXCreateFontA(device, 15, 0, FW_BOLD, 1, FALSE,
                    DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                    DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
                    "Arial", &m_statFont);

    m_engineFacade.onCreateDevice(device);

    m_stars->onCreateDevice(device);
    m_ship.onCreateDevice(device);
}

void DemoScene::onDestroyDevice()
{
    SAFE_RELEASE(m_statFont);
    m_engineFacade.onDestroyDevice();

    m_stars->onDestroyDevice();
    m_ship.onDestroyDevice();
}

void DemoScene::onLostDevice()
{
    m_engineFacade.onLostDevice();

    m_background.onLostDevice();
    m_stars->onLostDevice();
    m_ship.onLostDevice();
}

void DemoScene::onResetDevice(Device* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc)
{
    m_engineFacade.onResetDevice();
    m_size = Vec2((float)pBackBufferSurfaceDesc->Width, (float)pBackBufferSurfaceDesc->Height);
    m_camera.setViewSize(m_size);

    m_stars->onResetDevice(pd3dDevice, pBackBufferSurfaceDesc);
    m_background.onResetDevice(pd3dDevice, pBackBufferSurfaceDesc);
    m_ship.onResetDevice(pd3dDevice, pBackBufferSurfaceDesc);
}

void DemoScene::render(IDirect3DDevice9* pd3dDevice)
{
    pd3dDevice->Clear(0, 0, D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET, 0x0, 1.0f, 0);
    pd3dDevice->BeginScene();

    m_background.draw(this, &m_camera, pd3dDevice);
    m_stars->draw(this, &m_camera, pd3dDevice);
    m_ship.draw(this, &m_camera, pd3dDevice);

    RECT rect = {0, 0, 0, 0};
    
    m_statFont->DrawTextW(0, DXUTGetFrameStats(true), -1, &rect, DT_NOCLIP, 0xFFFFFFFF);

    pd3dDevice->EndScene();
}

void DemoScene::frameMove(float deltaTime)
{
    m_camera.setPos(m_ship.position);
    m_stars->update(deltaTime, &m_camera);
    m_ship.update(deltaTime, &m_camera);
}

void DemoScene::handleMsg(HWND hWnd, UINT msg, WPARAM, LPARAM)
{
}

void DemoScene::setDragging(bool isDragging)
{
    m_isDragged = isDragging;
}
