#include "DXUT.h"
#include "Sphere.h"
#include "types.h"

namespace
{
    struct SphereVertex
    {
        Vec3 pos;
        Vec3 normal;
        Vec2 tex;
    };
}

ID3DXMesh* GenerateSphere(IDirect3DDevice9* device, float radius, UINT rings, UINT sectors)
{
	if (!device)
	{
		return 0;
	}
	const DWORD numVertexes = rings * sectors;
	const DWORD numFaces = (rings - 1) * (sectors - 1) * 2;
    
    ID3DXMesh* mesh = 0;

    HRESULT hr = D3DXCreateMeshFVF(numFaces,
                      numVertexes,
                      D3DXMESH_32BIT | D3DXMESH_VB_WRITEONLY | D3DXMESH_IB_WRITEONLY,
                      D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1,
                      device,
                      &mesh);
    if (FAILED(hr))
    {
        MessageBox(0, L"Unable to generate sphere", L"Application error", MB_ICONSTOP);
        return 0;
    }
    SphereVertex* vertexes = 0;
    DWORD* indexes = 0;

    mesh->LockVertexBuffer(0, (void**)&vertexes);
    mesh->LockIndexBuffer(0, (void**)&indexes);


    float const R = 1.f / (float)(rings - 1);
    float const S = 1.f / (float)(sectors - 1);

    //
    for (UINT ring = 0; ring < rings; ring++)
    {
        for (UINT sector = 0; sector < sectors; sector++)
        {
            float const y = sin(D3DX_PI * 0.5f + D3DX_PI * ring * R );
            float const x = cos(2 * D3DX_PI * sector * S) * sin( D3DX_PI * ring * R );
            float const z = sin(2 * D3DX_PI * sector * S) * sin( D3DX_PI * ring * R );

            SphereVertex* vertex = vertexes++;

            vertex->pos = Vec3(x, y, z) * radius;
            vertex->pos *= radius;
            //
            D3DXVec3Normalize(&vertex->normal, &vertex->pos);
            vertex->tex = Vec2(sector / (float)(sectors - 1.0f),
                               ring / (float)(rings - 1.0f));
        }
    }
    //
    for (UINT ring = 0; ring < rings - 1; ++ring)
    {
        for (UINT sector = 0; sector < sectors - 1; ++sector)
        {
            DWORD v0 = ring * sectors + sector;
            DWORD v1 = ring * sectors + sector + 1;
            DWORD v2 = (ring + 1) * sectors + sector;
            DWORD v3 = (ring + 1) * sectors + sector + 1;

            *indexes++ = v0;
            *indexes++ = v1;
            *indexes++ = v2;

            *indexes++ = v2;
            *indexes++ = v1;
            *indexes++ = v3;
        }
    }

    mesh->UnlockIndexBuffer();
    mesh->UnlockVertexBuffer();
    return mesh;
}