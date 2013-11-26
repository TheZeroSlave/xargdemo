#include "DXUT.h"
#include "SceneCamera.h"

SceneCamera::SceneCamera()
{
    m_pos.x = m_pos.y = 1.0f;

    buildViewMatrix();
    buildProjMatrix();
}

void SceneCamera::setViewSize(const Vec2& sz)
{
    m_size = sz;
    buildProjMatrix();
}

void SceneCamera::moveTo(const Vec2& pos)
{
    m_pos = pos;
    buildViewMatrix();
}

void SceneCamera::buildViewMatrix()
{
    const Vec3 eyePos = Vec3(m_pos.x, m_pos.y, -1.0f);
    const Vec3 lookAt = Vec3(m_pos.x, m_pos.y, 1.0f);

    D3DXMatrixLookAtLH(&m_view, &eyePos, &lookAt, &Vec3(0, 1.0f, 0));
}

void SceneCamera::buildProjMatrix()
{
    const float znear = 0.1f;
    const float zfar = 5000.0f;

    D3DXMatrixOrthoLH(&m_proj, m_size.x, m_size.y, znear, zfar);
}