#pragma once
#include "types.h"

//2D ortho camera
class SceneCamera
{
public:
    SceneCamera();

    void setViewSize(const Vec2& sz);
    void moveTo(const Vec2& pos);
    void zoom(float factor);

    const Matrix& viewMatrix() const
    {
        return m_view;
    }

    const Matrix& projMatrix() const
    {
        return m_proj;
    }

    Vec2 viewSize() const
    {
        return m_size;
    }

    float maxZoom() const
    {
        return 2.0f;
    }

    Vec2 pos() const
    {
        return m_pos;
    }

    void setPos(const Vec2& p)
    {
        m_pos = p;
        buildViewMatrix();
    }

private:
    Matrix m_view, m_proj;

    Vec2 m_pos;
    Vec2 m_size;
    void buildViewMatrix();
    void buildProjMatrix();
};