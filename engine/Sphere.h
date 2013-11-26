#pragma once
#include "types.h"

Mesh* GenerateSphere(IDirect3DDevice9* device, float radius, UINT rings, UINT sectors);
Mesh* GenerateCylinder(IDirect3DDevice9* device, float radius, float height, UINT sectors);
