#include "DXUT.h"
#include "Camera.h"

FpsCamera::FpsCamera(const Vec3& eyePos, const Vec3& lookAt)
{
    setViewParams(eyePos, lookAt);
}

FpsCamera::FpsCamera()
{
    D3DXMatrixIdentity(&m_viewMatrix);
    D3DXMatrixIdentity(&m_projMatrix);
}

void FpsCamera::setViewParams(const Vec3& eyePos, const Vec3& lookAt)
{
    D3DXMatrixIdentity(&m_viewMatrix);
    D3DXMatrixIdentity(&m_projMatrix);

    m_view = lookAt - eyePos;
    D3DXVec3Cross(&m_right, &Vec3(0, 1, 0), &m_view);
    D3DXVec3Cross(&m_up, &m_view, &m_right);
    m_pos = eyePos;

    normalize();
    buildView();
}

void FpsCamera::setProjectionParams(float aspect, float fov, float zn, float zf)
{
    D3DXMatrixPerspectiveFovLH(&m_projMatrix, fov, aspect, zn, zf);
    m_near = zn;
}

void FpsCamera::normalize()
{
    D3DXVec3Normalize(&m_up, &m_up);
    D3DXVec3Normalize(&m_view, &m_view);
    D3DXVec3Normalize(&m_right, &m_right);
}

void FpsCamera::move(Direction d)
{
    Vec3 dir(0, 0, 0);
    if (d & Forward)
    {
        dir += m_view;
    }
    if (d & Backward)
    {
        dir -= m_view;
    }
    if (d & Rightward)
    {
        dir += m_right;
    }
    if (d & Leftward)
    {
        dir -= m_right;
    }
    D3DXVec3Normalize(&dir, &dir);
    m_pos += (dir * m_delta);

    buildView();
}

void FpsCamera::rotate(float x, float y)
{
    Matrix rotateX, rotateY;
    D3DXMatrixRotationAxis(&rotateX, &m_right, x * m_delta);
    D3DXMatrixRotationY(&rotateY, y * m_delta);
    Matrix total;
    D3DXMatrixMultiply(&total, &rotateX, &rotateY);
    D3DXVec3TransformNormal(&m_right, &m_right, &total);
    D3DXVec3TransformNormal(&m_up, &m_up, &total);
    D3DXVec3TransformNormal(&m_view, &m_view, &total);
    normalize();
    buildView();
}

void FpsCamera::frameMove(float delta)
{
    m_delta = delta;
}

void FpsCamera::buildView()
{
    m_viewMatrix._11 = m_right.x;
	m_viewMatrix._21 = m_right.y;
	m_viewMatrix._31 = m_right.z;

	m_viewMatrix._12 = m_up.x;
	m_viewMatrix._22 = m_up.y;
	m_viewMatrix._32 = m_up.z;

	m_viewMatrix._13 = m_view.x;
	m_viewMatrix._23 = m_view.y;
	m_viewMatrix._33 = m_view.z;

	m_viewMatrix._41 = -D3DXVec3Dot(&m_pos, &m_right);
	m_viewMatrix._42 = -D3DXVec3Dot(&m_pos, &m_up);
	m_viewMatrix._43 = -D3DXVec3Dot(&m_pos, &m_view);
	m_viewMatrix._44 = 1.0f;
}