#pragma once
#include "engine/utils.h"
#include "engine/SceneObject.h"
#include "SceneEffect.h"
#include "SpaceObject.h"

class Ship : public SpaceObject
{
public:
    Ship();
    void onCreateDevice(Device* device);
    void onDestroyDevice();

    void draw(Scene* scene, SceneCamera* camera, Device* device);
    void update(float frameDelta, SceneCamera* sceneCamera);

    void onLostDevice();
    void onResetDevice(Device* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc);

    void loadFromFile(const char* filename);
private:
    Matrix m_transform;
    Matrix m_rotation;
    Texture* m_texture;
    SceneEffect* m_effect;
    Vec2 m_dir;
    float m_angle;
    float m_angularA;
    float m_angularVelocity;
    float m_maxAngularSpeed;
    Vec2  m_size;
    float m_engineForce;

    void computeAngular(float dt);
    void computeTranslation(float dt);
};