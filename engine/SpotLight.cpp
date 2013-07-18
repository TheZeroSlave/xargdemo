#include "DXUT.h"
#include "SpotLight.h"

void SpotLight::setViewParams(const Vec3& pos, const Vec3& dir)
{
    m_pos = Vec4(pos.x, pos.y, pos.z, 1.0f);
    m_dir = Vec4(dir.x, dir.y, dir.z, 0.0f);

    m_camera.setViewParams(pos, pos - dir);
}

void SpotLight::setProjParams(float aspect, float fov, float zn, float zf)
{
    m_camera.setProjectionParams(aspect, fov, zn, zf);
}