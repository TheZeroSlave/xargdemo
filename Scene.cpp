#include "DXUT.h"

#include <algorithm>

#include "Scene.h"
#include "engine/Sphere.h"
#include "engine/Frustum.h"

namespace
{
    const DWORD shadowMapSize = 512;
    //
    struct CommonMeshVertex
    {
        Vec3 pos;
        Vec3 normal;
        Vec2 tex;

        CommonMeshVertex(const Vec3& p, const Vec3& n, const Vec2& t) : pos(p), normal(n), tex(t)
        {
        }
    };
    const DWORD CommonMeshFVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;

    Vec4 ComputePlane(Vec3 a, Vec3 b, Vec3 c)
    {
        Plane o;
        D3DXPlaneFromPoints(&o, &a, &b, &c);
        return Vec4(o.a, o.b, o.c, o.d);
    }

    void findMinMaxZValues(const std::vector<Vec3>& points, float& minLen, float& maxLen)
    {
        maxLen = -99999999999.f;
        minLen =  99999999999.0f;
        for (size_t i = 0; i < points.size(); ++i)
        {
            float projLen = points[i].z;

            if (projLen < minLen)
            {
                minLen = projLen;
            }
            if (projLen > maxLen)
            {
                maxLen = projLen;
            }
        }
    }

    struct LineVertex
    {
        Vec3 pos;
        DWORD color;
        LineVertex(const Vec3& p, DWORD c) : pos(p), color(c)
        {
        }

        LineVertex()
        {
        }
    };

    enum Controls
    {
        IDC_CHECK_RENDER_LIGHT_FRUSTUM,
        IDC_CHECK_USE_NOISE_TEXTURE,
        IDC_CHECK_USE_ENVIRONMENT,
        IDC_CHECK_USE_SPECULAR,
        IDC_LABEL_SPECULAR_FACTOR,
        IDC_SLIDER_SPECULAR,
        IDC_LABEL_SHAFTS_NUMBER,
        IDC_SLIDER_SHAFTS_NUMBER,
    };

    WCHAR SPECULAR_FACTOR_LABEL_STRING[] = L"Specular factor: %d";
    WCHAR SHAFTS_NUMBER_LABEL_STRING[] = L"Planes for shafts: %d";

    void CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext )
    {
        Scene* scene = (Scene*)pUserContext;
        
        switch (nControlID)
        {
        case IDC_CHECK_RENDER_LIGHT_FRUSTUM:
            scene->setLightFrustumVisible(((CDXUTCheckBox* )pControl)->GetChecked());
            break;

        case IDC_CHECK_USE_NOISE_TEXTURE:
            scene->setUseNoise(((CDXUTCheckBox* )pControl)->GetChecked());
            break;

        case IDC_CHECK_USE_ENVIRONMENT:
            scene->setUseEnvironment(((CDXUTCheckBox* )pControl)->GetChecked());
            break;

        case IDC_CHECK_USE_SPECULAR:
            scene->setUseSpecular(((CDXUTCheckBox* )pControl)->GetChecked());
            break;

        case IDC_SLIDER_SPECULAR:
            scene->setSpecularFactor(((CDXUTSlider* )pControl)->GetValue());
            break;

        case IDC_SLIDER_SHAFTS_NUMBER:
            scene->setShaftsNumber(((CDXUTSlider* )pControl)->GetValue());
            break;
        }
    }

    const int SPECULAR_FACTOR_MIN = 0;
    const int SPECULAR_FACTOR_MAX = 30;

    const int SHAFTS_NUMBER_MIN = 10;
    const int SHAFTS_NUMBER_MAX = 150;
}



Scene::Scene() : m_mouseDragging(false), m_ballMesh(0), m_ballTexture(0),
    m_poolTexture(0), m_spotTexture(0), m_noiseTexture1(0), m_noiseTexture2(0),
    m_cubeTexture(0), m_depthTexture(0), m_depthSurface(0), m_lightFrustumVisible(false),
    m_specularFactor(10), m_shaftsNumber(100)
{
    m_camera.setViewParams(Vec3(0, 1, -5), Vec3(0, 0, 1));
    m_light.setViewParams(Vec3(1.5, 3, 0), Vec3(0.5f, 1.f, 0.f));
    m_light.setProjParams(1.0f, D3DX_PI / 3.0f, 0.1f, 7.0f);
    initGUI();
}

void Scene::setLightFrustumVisible(bool vis)
{
    m_lightFrustumVisible = vis;
}

void Scene::setUseNoise(bool useNoise)
{
    m_effect->SetBool(m_effect->GetParameterByName(0, "g_useNoise"),
                      useNoise);
}

void Scene::setShaftsNumber(int number)
{
    m_shaftsNumber = number;

    m_effect->SetFloat(m_effect->GetParameterByName(0, "g_shaftOpacity"),
                       2.0f / number);

    wchar_t bufferStr[1024];
    swprintf_s(bufferStr, SHAFTS_NUMBER_LABEL_STRING, m_shaftsNumber);
    m_dialog.GetStatic(IDC_LABEL_SHAFTS_NUMBER)->SetText(bufferStr);
}

void Scene::setUseEnvironment(bool useEnv)
{
    m_effect->SetBool(m_effect->GetParameterByName(0, "g_useCubeEnv"),
                     useEnv);
}

void Scene::setUseSpecular(bool useSpecular)
{
    m_effect->SetBool(m_effect->GetParameterByName(0, "g_useSpecular"),
                      useSpecular);
}

void Scene::setSpecularFactor(int factor)
{
    m_specularFactor = factor;
    m_effect->SetInt(m_effect->GetParameterByName(0, "g_specFactor"),
                     factor);

    //update GUI label
    wchar_t bufferStr[1024];
    swprintf_s(bufferStr, SPECULAR_FACTOR_LABEL_STRING, m_specularFactor);
    m_dialog.GetStatic(IDC_LABEL_SPECULAR_FACTOR)->SetText(bufferStr);
}

Scene::~Scene()
{
}

void Scene::initGUI()
{
    m_dialog.Init(&m_resManager);
    m_dialog.SetFont(0, L"Tahoma", 15, FW_NORMAL);
    m_dialog.SetCallback(OnGUIEvent, this);
    
    int yButtonPos = 25;

    m_dialog.AddCheckBox(IDC_CHECK_RENDER_LIGHT_FRUSTUM,
                         L"Render light frustum",
                         0,
                         yButtonPos,
                         150,
                         24);
    yButtonPos += 25;

    m_dialog.AddCheckBox(IDC_CHECK_USE_NOISE_TEXTURE,
                         L"Use noise in light shafts",
                         0,
                         yButtonPos,
                         160,
                         24,
                         TRUE);

    yButtonPos += 25;

    m_dialog.AddCheckBox(IDC_CHECK_USE_ENVIRONMENT,
                         L"Use environment map",
                         0,
                         yButtonPos,
                         150,
                         24,
                         TRUE);
    yButtonPos += 25;

    m_dialog.AddCheckBox(IDC_CHECK_USE_SPECULAR,
                         L"Use specular lighting",
                         0,
                         yButtonPos,
                         150,
                         24,
                         TRUE);

    yButtonPos += 40;
    WCHAR bufferStr[1024];
    swprintf_s(bufferStr, SPECULAR_FACTOR_LABEL_STRING, m_specularFactor);

    m_dialog.AddStatic(IDC_LABEL_SPECULAR_FACTOR,
                       bufferStr,
                       0,
                       yButtonPos,
                       150,
                       20);

    yButtonPos += 25;

    m_dialog.AddSlider(IDC_SLIDER_SPECULAR,
                       3,
                       yButtonPos,
                       150,
                       20,
                       SPECULAR_FACTOR_MIN,
                       SPECULAR_FACTOR_MAX,
                       m_specularFactor);

    yButtonPos += 50;

    swprintf_s(bufferStr, SHAFTS_NUMBER_LABEL_STRING, m_shaftsNumber);

    m_dialog.AddStatic(IDC_LABEL_SHAFTS_NUMBER,
                       bufferStr,
                       0,
                       yButtonPos,
                       150,
                       20);
    yButtonPos += 25;

    m_dialog.AddSlider(IDC_SLIDER_SHAFTS_NUMBER,
                       3,
                       yButtonPos,
                       150,
                       20,
                       SHAFTS_NUMBER_MIN,
                       SHAFTS_NUMBER_MAX,
                       m_shaftsNumber);
                       
}

void Scene::onCreateDevice(IDirect3DDevice9* pd3dDevice)
{
    ID3DXBuffer* bufferError;
	if (FAILED(D3DXCreateEffectFromFile(pd3dDevice, _T("Render.fx"), 0, 0, 0, 0, &m_effect, &bufferError)))
	{
		if (!bufferError)
		{
			MessageBox(0, _T("Unable to find effect file"), _T("Application error"), MB_ICONSTOP);
			return;
		}
		const char* bufferErrorStr = (char*)bufferError->GetBufferPointer();
		MessageBoxA(0, bufferErrorStr, "Error while compiling effect", MB_ICONSTOP);
		return;
	}
	
	m_worldHandle = m_effect->GetParameterByName(0, "g_world");
    m_viewHandle = m_effect->GetParameterByName(0, "g_view");
    m_projHandle = m_effect->GetParameterByName(0, "g_proj");
    m_invViewHandle = m_effect->GetParameterByName(0, "g_invView");

    m_ballTexHandle = m_effect->GetParameterByName(0, "g_diffuseTexture");
    m_eyePosHandle = m_effect->GetParameterByName(0, "g_eye");
    m_lightPosHandle = m_effect->GetParameterByName(0, "g_lightPos");
    m_wvpHandle = m_effect->GetParameterByName(0, "g_wvp");
    m_cubeTexHandle = m_effect->GetParameterByName(0, "g_cubeTexture");
    m_lightDirHandle = m_effect->GetParameterByName(0, "g_lightDir");
    m_shaftPosHandle = m_effect->GetParameterByName(0, "g_shaftPos");
    m_textureMatrixHandle = m_effect->GetParameterByName(0, "g_textureMatrix");
    m_noiseOffsetHandle = m_effect->GetParameterByName(0, "g_noiseOffset");
    m_noiseTexture1Handle = m_effect->GetParameterByName(0, "g_noiseTexture1");
    m_noiseTexture2Handle = m_effect->GetParameterByName(0, "g_noiseTexture2");
    m_clipPlanesHandle = m_effect->GetParameterByName(0, "g_clipPlanes");
    m_shadowTexHandle = m_effect->GetParameterByName(0, "g_shadowTexture");
    m_lightMatrixHandle = m_effect->GetParameterByName(0, "g_lightMatrix");

    m_skyboxTechHandle = m_effect->GetTechniqueByName("RenderSkyboxTech");
    m_sphereTechHandle = m_effect->GetTechniqueByName("RenderSphereTech");
    m_sceneTechHandle = m_effect->GetTechniqueByName("RenderSceneTech");
    m_shaftsTechHandle = m_effect->GetTechniqueByName("RenderShaftsTech");
    m_shadowTechHandle = m_effect->GetTechniqueByName("RenderShadowTech");

    setUseNoise(true);
    setUseEnvironment(true);
    setUseSpecular(true);
    setSpecularFactor(m_specularFactor);
    setShaftsNumber(m_shaftsNumber);

	SAFE_RELEASE(bufferError);

    m_resManager.OnD3D9CreateDevice(pd3dDevice);

    return;
}

void Scene::onDestroyDevice()
{
    SAFE_RELEASE(m_effect);
    m_resManager.OnD3D9DestroyDevice();
}

void Scene::createScene(IDirect3DDevice9* pd3dDevice)
{
    const int NUM_OF_RINGS = 30;
    const int NUM_OF_SEGMENTS = 30;

    m_ballMesh = GenerateSphere(pd3dDevice, 1.0f, NUM_OF_RINGS, NUM_OF_SEGMENTS);
    createFloor(pd3dDevice);
    createShafts(pd3dDevice);
}

void Scene::createFloor(IDirect3DDevice9* pd3dDevice)
{
    const float inf = 1000.0f;
    const float yPos = -1.0f;

    D3DXCreateMeshFVF(2,
                      4,
                      D3DXMESH_VB_WRITEONLY | D3DXMESH_IB_WRITEONLY,
                      CommonMeshFVF,
                      pd3dDevice,
                      &m_floorMesh);
    CommonMeshVertex* vertexes = 0;

    m_floorMesh->LockVertexBuffer(0, (void**)&vertexes);
    vertexes[0].pos = Vec3(-inf, yPos, inf);
    vertexes[1].pos = Vec3(inf, yPos, inf);
    vertexes[2].pos = Vec3(-inf, yPos, -inf);
    vertexes[3].pos = Vec3(inf, yPos, -inf);

    vertexes[0].tex = Vec2(0, 0);
    vertexes[1].tex = Vec2(inf, 0);
    vertexes[2].tex = Vec2(0, inf);
    vertexes[3].tex = Vec2(inf, inf);

    vertexes[0].normal =
        vertexes[1].normal =
        vertexes[2].normal =
        vertexes[3].normal = Vec3(0, 1, 0);

    m_floorMesh->UnlockVertexBuffer();

    WORD* indexes = 0;
    m_floorMesh->LockIndexBuffer(0, (void**)&indexes);

    *indexes++ = 0;
    *indexes++ = 1;
    *indexes++ = 2;
    *indexes++ = 2;
    *indexes++ = 1;
    *indexes++ = 3;

    m_floorMesh->UnlockIndexBuffer();
}

void Scene::createShafts(IDirect3DDevice9* pd3dDevice)
{
    D3DXCreateMeshFVF(2,
                      4,
                      D3DXMESH_VB_WRITEONLY | D3DXMESH_IB_WRITEONLY,
                      D3DFVF_XYZ,
                      pd3dDevice,
                      &m_shaftMesh);
    
    Vec3* vertexes = 0;
    m_shaftMesh->LockVertexBuffer(0, (void**)&vertexes);

    *vertexes++ = Vec3(-400.5f, 400.5f, 0);
    *vertexes++ = Vec3(400.5f, 400.5f, 0);
    *vertexes++ = Vec3(-400.5f, -400.5f, 0);
    *vertexes++ = Vec3(400.5f, -400.5f, 0);

    m_shaftMesh->UnlockVertexBuffer();

    WORD* indexes = 0;
    m_shaftMesh->LockIndexBuffer(0, (void**)&indexes);

    *indexes++ = 0;
    *indexes++ = 1;
    *indexes++ = 2;
    *indexes++ = 2;
    *indexes++ = 1;
    *indexes++ = 3;

    m_shaftMesh->UnlockVertexBuffer();
}

void Scene::createDepthSurface(IDirect3DDevice9* pd3dDevice)
{
    pd3dDevice->CreateTexture(shadowMapSize,
                              shadowMapSize,
                              1,
                              D3DUSAGE_RENDERTARGET,
                              D3DFMT_R32F,
                              D3DPOOL_DEFAULT,
                              &m_depthTexture,
                              0);
    pd3dDevice->CreateDepthStencilSurface(shadowMapSize,
        shadowMapSize,
        D3DFMT_D24X8,
        D3DMULTISAMPLE_NONE,
        0,
        false,
        &m_depthSurface,
        0);
}

void Scene::onLostDevice()
{
    SAFE_RELEASE(m_ballMesh);
	SAFE_RELEASE(m_ballTexture);
    SAFE_RELEASE(m_poolTexture);
    SAFE_RELEASE(m_floorMesh);
    SAFE_RELEASE(m_shaftMesh);
    SAFE_RELEASE(m_cubeTexture);
    SAFE_RELEASE(m_spotTexture);
    SAFE_RELEASE(m_noiseTexture1);
    SAFE_RELEASE(m_noiseTexture2);
    SAFE_RELEASE(m_depthTexture);
    SAFE_RELEASE(m_depthSurface);

	m_effect->OnLostDevice();
    m_resManager.OnD3D9LostDevice();
}

void Scene::onResetDevice(IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc)
{
    D3DXMATRIX world;

	D3DXMatrixIdentity(&world);
	pd3dDevice->SetTransform(D3DTS_WORLD, &world);
    pd3dDevice->SetTransform(D3DTS_VIEW, &world);
    pd3dDevice->SetTransform(D3DTS_PROJECTION, &world);

    const float zn = 0.1f;
    const float zf = 100.0f;
    m_camera.setProjectionParams(
		pBackBufferSurfaceDesc->Width / (float)pBackBufferSurfaceDesc->Height,
        D3DX_PI / 4.0f,
		zn,
		zf);

    createScene(pd3dDevice);
    createDepthSurface(pd3dDevice);

	D3DXCreateTextureFromFile(pd3dDevice, _T("textures\\ball.png"), &m_ballTexture);
    D3DXCreateTextureFromFile(pd3dDevice, _T("textures\\pool.jpg"), &m_poolTexture);
    D3DXCreateTextureFromFile(pd3dDevice, _T("textures\\spot.png"), &m_spotTexture);
    D3DXCreateTextureFromFile(pd3dDevice, _T("textures\\noise1.bmp"), &m_noiseTexture1);
    D3DXCreateTextureFromFile(pd3dDevice, _T("textures\\noise2.bmp"), &m_noiseTexture2);
    D3DXCreateCubeTextureFromFile(pd3dDevice, _T("textures\\cubetexture.dds"), &m_cubeTexture);

	m_effect->OnResetDevice();
    m_resManager.OnD3D9ResetDevice();
    //
    m_dialog.SetSize(pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height);
    m_dialog.SetLocation(0, 0);
}

void Scene::renderSceneToShadowBuffer(IDirect3DDevice9* pd3dDevice)
{
    IDirect3DSurface9* pOldTarget = 0, *pShadowSurface = 0, *pOldDepth;
    m_depthTexture->GetSurfaceLevel(0, &pShadowSurface);
    pd3dDevice->GetRenderTarget(0, &pOldTarget);
    pd3dDevice->GetDepthStencilSurface(&pOldDepth);

    pd3dDevice->SetRenderTarget(0, pShadowSurface);
    pd3dDevice->SetDepthStencilSurface(m_depthSurface);
    
    const float bigValue = 9999999999.0f;
    pd3dDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, *((DWORD*)&bigValue), 1.0f, 0);
    pd3dDevice->BeginScene();
    
    const Matrix& viewProj = m_light.viewMatrix() * m_light.projMatrix();
    
    m_effect->SetMatrix(m_lightMatrixHandle, &viewProj);
    m_effect->SetMatrix(m_wvpHandle, &viewProj);

    m_effect->SetTechnique(m_shadowTechHandle);
    
    m_effect->Begin(0, 0);
    m_effect->BeginPass(0);

    m_floorMesh->DrawSubset(0);
    m_ballMesh->DrawSubset(0);

    m_effect->EndPass();
    m_effect->End();
    
    pd3dDevice->EndScene();

    pd3dDevice->SetRenderTarget(0, pOldTarget);
    pd3dDevice->SetDepthStencilSurface(pOldDepth);

    SAFE_RELEASE(pOldTarget);
    SAFE_RELEASE(pShadowSurface);
    SAFE_RELEASE(pOldDepth);
}

Matrix Scene::ballRotationMatrix() const
{
    Matrix rotation;
    static float t = 0.00;
    D3DXMatrixRotationY(&rotation, t);
    t += 0.01f;
    return rotation;
}

void Scene::render(IDirect3DDevice9* pd3dDevice)
{
    renderSceneToShadowBuffer(pd3dDevice);

    // Clear the render target and the zbuffer 
    pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB( 0, 0, 0, 0 ), 1.0f, 0 );

    pd3dDevice->BeginScene();
    
    Matrix worldM = ballRotationMatrix();

    pd3dDevice->SetTransform(D3DTS_VIEW, m_camera.viewMatrix());
    pd3dDevice->SetTransform(D3DTS_PROJECTION, m_camera.projMatrix());

    m_effect->SetMatrix(m_worldHandle, &worldM);
    m_effect->SetMatrix(m_viewHandle, m_camera.viewMatrix());
    m_effect->SetMatrix(m_projHandle, m_camera.projMatrix());

    Matrix invView = *m_camera.viewMatrix();
    D3DXMatrixInverse(&invView, 0, &invView);
    m_effect->SetMatrix(m_invViewHandle, &invView);

    m_effect->SetFloatArray(m_eyePosHandle, m_camera.eyePos(), 3);
    m_effect->SetVector(m_lightPosHandle, &m_light.pos());
    m_effect->SetVector(m_lightDirHandle, &m_light.dir());
    m_effect->SetTexture(m_cubeTexHandle, m_cubeTexture);
    m_effect->SetTexture(m_ballTexHandle, m_ballTexture);

    renderSphere(pd3dDevice);

    D3DXMatrixIdentity(&worldM);
    m_effect->SetMatrix(m_worldHandle, &worldM);
    
    renderFloor(pd3dDevice);
    renderShafts(pd3dDevice);
    
    m_dialog.OnRender(m_deltaTime);

    pd3dDevice->EndScene();
}

void Scene::renderShafts(IDirect3DDevice9* pd3dDevice)
{
    Matrix viewProj, invViewProj;

    viewProj = m_light.viewMatrix() * m_light.projMatrix();
    D3DXMatrixInverse(&invViewProj, 0, &viewProj);

    std::vector<Vec3> clipFrustumPoints = Frustum::getPointsFromMatrix(invViewProj);

    std::vector<Vec4> planes;
    planes.push_back(ComputePlane(clipFrustumPoints[0], clipFrustumPoints[4], clipFrustumPoints[2]));//left
    planes.push_back(ComputePlane(clipFrustumPoints[3], clipFrustumPoints[7], clipFrustumPoints[1]));//right
    planes.push_back(ComputePlane(clipFrustumPoints[6], clipFrustumPoints[7], clipFrustumPoints[2]));//bottom
    planes.push_back(ComputePlane(clipFrustumPoints[0], clipFrustumPoints[1], clipFrustumPoints[4]));//top
    planes.push_back(ComputePlane(clipFrustumPoints[1], clipFrustumPoints[0], clipFrustumPoints[3]));//near
    planes.push_back(ComputePlane(clipFrustumPoints[4], clipFrustumPoints[5], clipFrustumPoints[6]));//far

    if (m_lightFrustumVisible)
    {
        pd3dDevice->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);


        std::vector<LineVertex> vertexes;
        vertexes.resize(clipFrustumPoints.size());

        //this code works only in msvc2010/2012
        std::transform(clipFrustumPoints.begin(),
                       clipFrustumPoints.end(),
                       vertexes.begin(),
                       [](const Vec3& v) { return LineVertex(v, D3DCOLOR_XRGB(255, 255, 0)); });
        //
        WORD indexes[] = {0, 1, 1, 3, 3, 2, 2, 0, 0, 4, 1, 5, 2, 6, 3, 7, 4, 5, 5, 7, 7, 6, 6, 4};
        pd3dDevice->SetRenderState(D3DRS_LIGHTING, false);
        pd3dDevice->DrawIndexedPrimitiveUP(D3DPT_LINELIST, 0, 8, 12, indexes, D3DFMT_INDEX16, &vertexes[0], sizeof(LineVertex));
    }

    //clip planes via fixed pipeline got some issues with points are behind clip space
    //so put them in shader
    m_effect->SetVectorArray(m_clipPlanesHandle, &planes[0], planes.size());
    D3DXVec3TransformCoordArray(&clipFrustumPoints[0],
                                sizeof(Vec3),
                                &clipFrustumPoints[0],
                                sizeof(Vec3),
                                m_camera.viewMatrix(),
                                clipFrustumPoints.size());
    float maxLen, minLen;
    findMinMaxZValues(clipFrustumPoints, minLen, maxLen);

    pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
    pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
    pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

    Matrix textureMatrix, invView;
    invView = *m_camera.viewMatrix();
    D3DXMatrixInverse(&invView, 0, &invView);
    D3DXMatrixMultiply(&textureMatrix, &invView, &viewProj);

    m_effect->SetMatrix(m_textureMatrixHandle, &textureMatrix);
    m_effect->SetTexture(m_ballTexHandle, m_spotTexture);
    m_effect->SetTechnique(m_shaftsTechHandle);
    m_effect->SetTexture(m_shadowTexHandle, m_depthTexture);

    static float t = 0.0f;
    t+= 0.0005f;
    m_effect->SetFloat(m_noiseOffsetHandle, t);
    m_effect->SetTexture(m_noiseTexture1Handle, m_noiseTexture1);
    m_effect->SetTexture(m_noiseTexture2Handle, m_noiseTexture2);

    m_effect->Begin(0, 0);
    m_effect->BeginPass(0);
    
    const float shaftStep = (maxLen - minLen) / m_shaftsNumber;

    for (int i = m_shaftsNumber; i >=0 ; i--)
    {
        m_effect->SetFloat(m_shaftPosHandle, minLen + shaftStep * i);
        m_effect->CommitChanges();
        m_shaftMesh->DrawSubset(0);
    }

    m_effect->EndPass();
    m_effect->End();
    
    pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);

}

void Scene::renderFloor(IDirect3DDevice9* pd3dDevice)
{
    m_effect->SetTechnique(m_sceneTechHandle);
    m_effect->SetTexture(m_ballTexHandle, m_poolTexture);

    m_effect->Begin(0, 0);
    m_effect->BeginPass(0);

    m_floorMesh->DrawSubset(0);

    m_effect->EndPass();
    m_effect->End();
}

void Scene::renderSphere(IDirect3DDevice9* pd3dDevice)
{
    m_effect->SetTechnique(m_sphereTechHandle);
    
    UINT passes = 0;
    m_effect->Begin(&passes, 0);
    for (UINT pass = 0; pass < passes; pass++)
    {
        m_effect->BeginPass(pass);
        m_ballMesh->DrawSubset(0);
        m_effect->EndPass();
    }
    m_effect->End();
}

void Scene::handleMsg(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    m_resManager.MsgProc(hwnd, msg, wParam, lParam);
    if (m_dialog.MsgProc(hwnd, msg, wParam, lParam))
    {
        m_mouseDragging = false;
    }
}

void Scene::frameMove(float deltaTime)
{
    m_deltaTime = deltaTime;
    m_camera.frameMove(deltaTime);

    if (m_mouseDragging)
    {
        POINT currentPt;
        GetCursorPos(&currentPt);
        m_camera.rotate((currentPt.y - m_lastMousePt.y) * 0.5f, (currentPt.x - m_lastMousePt.x) * 0.5f);
        m_lastMousePt = currentPt;
    }

    int direction = FpsCamera::None;

    if (GetAsyncKeyState('W'))
    {
        direction |= FpsCamera::Forward;
    }
    if (GetAsyncKeyState('S'))
    {
        direction |= FpsCamera::Backward;
    }
    if (GetAsyncKeyState('A'))
    {
        direction |= FpsCamera::Leftward;
    }
    if (GetAsyncKeyState('D'))
    {
        direction |= FpsCamera::Rightward;
    }
    m_camera.move((FpsCamera::Direction)direction);
}
