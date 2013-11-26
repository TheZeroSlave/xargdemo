#include "engine\EmptyScene.h"
#include <atlbase.h>
#include "engine\Camera.h"
#include "SpaceBackground.h"
#include "Ship.h"
#include "EngineFacade.h"
#include "Stars.h"

class DemoScene : public Scene
{
public:
    DemoScene();
    ~DemoScene();

    void onCreateDevice(IDirect3DDevice9* pd3dDevice);
    void onDestroyDevice();

    void onLostDevice();
    void onResetDevice(IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc);
    //
    void render(IDirect3DDevice9* pd3dDevice);
    void frameMove(float deltaTime);
    void handleMsg(HWND hWnd, UINT msg, WPARAM, LPARAM);

    void setDragging(bool isDragging);

    Vec2 size() const
    {
        return m_size;
    }
    SceneCamera& camera()
    {
        return m_camera;
    }
private:
    bool m_isDragged;
    SceneCamera m_camera;
    SpaceBackground m_background;
    Ship m_ship;
    EngineFacade m_engineFacade;
    std::auto_ptr<Stars> m_stars;
    Font* m_statFont;

    Vec2 m_size;
};