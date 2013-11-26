#include "DXUT.h"
#include "Ship.h"

Ship::Ship() : SpaceObject(Vec2(0, 0)), m_texture(0), m_effect(0), m_angularA(0.0f),
    m_angularVelocity(0.0), m_angle(0.0f), m_maxAngularSpeed(2.0f), m_engineForce(0.0f)
{
    D3DXMatrixIdentity(&m_transform);
    D3DXMatrixIdentity(&m_rotation);
    m_size = Vec2(100, 62);
}


Texture* texture;
Texture* texture2;

void Ship::draw(Scene* scene, SceneCamera* camera, Device* device)
{
    m_effect->setBaseTexture(m_texture);
    Effect* effect = m_effect->dxEffect();

    m_effect->setWorldMatrix(m_transform);
    //apply view and proj params
    m_effect->applyCamera(*camera);

    effect->SetTechnique(effect->GetTechniqueByName("RenderShip"));
    effect->Begin(0, 0);

    effect->BeginPass(0);
    //DrawScreenSprite(device, 0, Vec2(220.0f - m_size.x * 0.5f, 240.0f - m_size.y * 0.5f), Vec2(m_size.x, m_size.y));
    DrawWorldSprite(device, 0, Vec3(-m_size.x * 0.5f, m_size.y * 0.5f, 0.5f), Vec3(m_size.x, -m_size.y, 0.5f));

    effect->EndPass();
    
    effect->End();
}

void Ship::loadFromFile(const char* filename)
{
    mass = 1.0f;
}

void Ship::update(float fDelta, SceneCamera* sceneCamera)
{
    m_angularA = 0.0f;
    m_engineForce = 0.0f;

    if (GetAsyncKeyState(VK_LEFT))
    {
        m_angularA = 1.0f;
    }
    if (GetAsyncKeyState(VK_RIGHT))
    {
        m_angularA = -1.0f;
    }
    if (GetAsyncKeyState('W'))
    {
        m_engineForce = 30.0f;
    }
    if (GetAsyncKeyState('S'))
    {
        m_engineForce = -30.0f;
    }

    computeAngular(fDelta);
    computeTranslation(fDelta);

    D3DXMatrixTranslation(&m_transform, position.x, position.y, 0.0f);
    D3DXMatrixMultiply(&m_transform, &m_rotation, &m_transform);

    SpaceObject::update(fDelta, sceneCamera);
}

void Ship::computeTranslation(float dt)
{
    float a = m_engineForce / mass;
    velocity += m_dir * a * dt;
    position += velocity * dt;
    if (fabs(a) < 0.01f)
    {
        velocity -= velocity * dt * 0.1f;
    }
}

void Ship::computeAngular(float dt)
{
    m_angularVelocity += m_angularA * dt;
    m_angularVelocity = Math::clamp(m_angularVelocity, -m_maxAngularSpeed, m_maxAngularSpeed);

    m_angle += m_angularVelocity * dt;
    D3DXMatrixRotationZ(&m_rotation, m_angle);

    if (fabs(m_angularA) < 0.01f)
    {
        m_angularVelocity -= m_angularVelocity * dt * 0.5f;
    }
    
    Vec3 dir(0, 1, 0);
    D3DXVec3TransformCoord(&dir, &dir, &m_rotation);
    m_dir = Vec2(dir.x, dir.y);
}

void Ship::onLostDevice()
{
    SAFE_RELEASE(m_texture);
    m_effect->dxEffect()->OnLostDevice();
}

void Ship::onCreateDevice(Device* device)
{
    m_effect = new SceneEffect(LoadEffectFromFile(device, "Data/render.fx"));
}

void Ship::onDestroyDevice()
{
    m_effect->release();
}

void Ship::onResetDevice(Device* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc)
{
    m_effect->dxEffect()->OnResetDevice();
    m_texture = LoadTextureFromFile(pd3dDevice, "Data/ships/ARC-170/base.png");
    D3DSURFACE_DESC desc;
    m_texture->GetLevelDesc(0, &desc);
}

