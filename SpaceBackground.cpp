#include "DXUT.h"
#include "SpaceBackground.h"
#include "engine/utils.h"

SpaceBackground::SpaceBackground() : m_texture(0)
{
}

void SpaceBackground::onResetDevice(IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc)
{
    m_texture = LoadTextureFromFile(pd3dDevice, "Data/background3.jpg");
}

void SpaceBackground::onLostDevice()
{
    SAFE_RELEASE(m_texture);
}

void SpaceBackground::draw(Scene* scene, SceneCamera* camera, Device* device)
{
    DrawScreenSprite(device, m_texture, Vec2(0, 0), camera->viewSize());
}