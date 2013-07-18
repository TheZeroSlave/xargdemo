#pragma once
#include "types.h"
#include "Camera.h"

class SpotLight
{
public:
    void setViewParams(const Vec3& pos, const Vec3& dir);
    void setProjParams(float aspect, float fov, float zn, float zf);

    Matrix viewMatrix() const
    {
        return *m_camera.viewMatrix();
    }

    Matrix projMatrix() const
    {
        return *m_camera.projMatrix();
    }

    Vec4 pos() const
    {
        return m_pos;
    }

    Vec4 dir() const
    {
        return m_dir;
    }
private:
    Vec4 m_pos;
    Vec4 m_dir;
    mutable FpsCamera m_camera;
};