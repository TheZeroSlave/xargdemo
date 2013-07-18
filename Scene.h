#pragma once
#include "DXUTgui.h"
#include "engine/Camera.h"
#include "engine/types.h"
#include "engine/SpotLight.h"
#include "engine/emptyscene.h"

class Scene : public EmptyScene
{
public:
    Scene();
    ~Scene();

    void onCreateDevice(IDirect3DDevice9* pd3dDevice);
    void onDestroyDevice();

    void onLostDevice();
    void onResetDevice(IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc);
    //
    void render(IDirect3DDevice9* pd3dDevice);
    void frameMove(float deltaTime);
    void handleMsg(HWND hWnd, UINT msg, WPARAM, LPARAM);

    void setDragging(bool isDragging)
    {
        m_mouseDragging = isDragging;
        GetCursorPos(&m_lastMousePt);
    }

    void setLightFrustumVisible(bool bVisible);
    void setUseNoise(bool useNoise);
    void setUseEnvironment(bool useEnvironment);
    void setUseSpecular(bool useSpecular);
    void setSpecularFactor(int factor);
    void setShaftsNumber(int number);
private:
    FpsCamera m_camera;
    bool      m_mouseDragging;
    POINT     m_lastMousePt;
    ID3DXMesh* m_ballMesh;
    float m_deltaTime;
    int m_specularFactor;
    int m_shaftsNumber;

    IDirect3DTexture9* m_ballTexture;
    IDirect3DTexture9* m_poolTexture;
    IDirect3DTexture9* m_spotTexture;
    IDirect3DTexture9* m_noiseTexture1;
    IDirect3DTexture9* m_noiseTexture2;
    IDirect3DCubeTexture9* m_cubeTexture;
    IDirect3DTexture9* m_depthTexture;
    IDirect3DSurface9* m_depthSurface;
    ID3DXMesh* m_floorMesh;
    ID3DXMesh* m_shaftMesh;
    ID3DXEffect* m_effect;
    bool m_lightFrustumVisible;

    D3DXHANDLE m_worldHandle;
    D3DXHANDLE m_viewHandle;
    D3DXHANDLE m_projHandle;
    D3DXHANDLE m_invViewHandle;

    D3DXHANDLE m_ballTexHandle;
    D3DXHANDLE m_eyePosHandle;
    D3DXHANDLE m_lightPosHandle;
    D3DXHANDLE m_lightDirHandle;
    D3DXHANDLE m_wvpHandle;
    D3DXHANDLE m_cubeTexHandle;
    D3DXHANDLE m_shaftPosHandle;
    D3DXHANDLE m_textureMatrixHandle;
    D3DXHANDLE m_noiseOffsetHandle;
    D3DXHANDLE m_noiseTexture1Handle;
    D3DXHANDLE m_noiseTexture2Handle;
    D3DXHANDLE m_clipPlanesHandle;
    D3DXHANDLE m_shadowTexHandle;
    D3DXHANDLE m_lightMatrixHandle;

    D3DXHANDLE m_skyboxTechHandle;
    D3DXHANDLE m_sphereTechHandle;
    D3DXHANDLE m_sceneTechHandle;
    D3DXHANDLE m_shaftsTechHandle;
    D3DXHANDLE m_shadowTechHandle;

    //
    SpotLight m_light;

    CDXUTDialogResourceManager  m_resManager;
    CDXUTDialog m_dialog;

    void createScene(IDirect3DDevice9* pd3dDevice);
    void createDepthSurface(IDirect3DDevice9* pd3dDevice);
    void createFloor(IDirect3DDevice9* pd3dDevice);
    void createShafts(IDirect3DDevice9* pd3dDevice);

    void initGUI();

    //
    void renderSceneToShadowBuffer(IDirect3DDevice9* pd3dDevice);
    void renderSphere(IDirect3DDevice9* pd3dDevice);
    void renderFloor(IDirect3DDevice9* pd3dDevice);
    void renderShafts(IDirect3DDevice9* pd3dDevice);

    Matrix ballRotationMatrix() const;
};
