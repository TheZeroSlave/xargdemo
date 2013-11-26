#pragma once
#include "engine\DeviceObject.h"
#include "engine\SceneObject.h"

class SpaceBackground : public SceneObject
{
public:
    SpaceBackground();

    void onLostDevice();
    void onResetDevice(IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc);
    void draw(Scene* scene, SceneCamera* camera, Device* device);
private:
    Texture* m_texture;
};