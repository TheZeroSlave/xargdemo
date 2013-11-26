#pragma once

#include "types.h"

class DeviceObject
{
public:
    virtual void onCreateDevice(Device* device){}
    virtual void onDestroyDevice(){}

    virtual void onLostDevice() = 0;
    virtual void onResetDevice(IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc) = 0;
};