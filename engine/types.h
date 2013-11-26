#pragma once

#include <d3dx9.h>
#include <tchar.h>

typedef D3DXVECTOR4 Vec4;
typedef D3DXVECTOR3 Vec3;
typedef D3DXVECTOR2 Vec2;
typedef D3DXMATRIX Matrix;
typedef D3DXPLANE Plane;

typedef IDirect3DTexture9 Texture;
typedef IDirect3DDevice9 Device;
typedef ID3DXEffect Effect;
typedef ID3DXBuffer DXBuffer;
typedef D3DXHANDLE DXHandle;
typedef ID3DXMesh Mesh;
typedef IDirect3DVertexBuffer9 VertexBuffer;
typedef IDirect3DIndexBuffer9 IndexBuffer;
typedef ID3DXFont Font;

struct ParticleVertex
{
    Vec3 pos;
    DWORD color;
};
#define ParticleVertexFVF (D3DFVF_XYZ | D3DFVF_DIFFUSE)