#pragma once
#include "types.h"

class FpsCamera
{
public:
    enum Direction
    {
        None = 0,
        Forward = 1,
        Backward = 2,
        Rightward = 4,
        Leftward = 8,
    };
    FpsCamera();
	FpsCamera(const Vec3& eyePos,
              const Vec3& lookAt);

    void setViewParams(const Vec3& eyePos,
                       const Vec3& lookAt);

    void setProjectionParams(float aspect, float fov, float zn, float zf);
    void frameMove(float delta);

    Matrix* viewMatrix()
    {
        return &m_viewMatrix;
    }

    Matrix* projMatrix()
    {
        return &m_projMatrix;
    }

    void move(Direction d);
    void rotate(float x, float y);

    Vec3 eyePos() const
    {
        return m_pos;
    }

    float nearPlane() const
    {
        return m_near;
    }

    Vec3 viewDir() const
    {
        return m_view;
    }
private:
    Vec3 m_right, m_up, m_view, m_pos;
    Matrix m_viewMatrix, m_projMatrix;
    float m_delta;
    float m_near;

    void normalize();
    void buildView();
    void buildProj();
};