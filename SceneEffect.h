#pragma once
#include "engine/types.h"
#include "engine/SceneCamera.h"

class SceneEffect
{
public:
    enum MatrixType
    {
        WorldMatrix,
        ViewMatrix,
        ProjMatrix,
    };
    SceneEffect(Effect* effect);

    void setSceneSize(const Vec2& sceneSize);
    void setBaseTexture(Texture* texture);

    void setIdentityMatrix(MatrixType type)
    {
        Matrix i;
        D3DXMatrixIdentity(&i);

        switch (type)
        {
        case WorldMatrix:
            setWorldMatrix(i);
            break;
        case ViewMatrix:
            setViewMatrix(i);
            break;
        case ProjMatrix:
            setProjMatrix(i);
            break;
        }
    }
    void setWorldMatrix(const Matrix& worldMatrix);
    void setViewMatrix(const Matrix& viewMatrix);
    void setProjMatrix(const Matrix& projMatrix);

    void applyCamera(const SceneCamera& camera);

    Effect* dxEffect() const
    {
        return m_effect;
    }

    void release()
    {
        SAFE_RELEASE(m_effect);
    }
private:
    Effect*  m_effect;
    DXHandle m_textureHandle;
    DXHandle m_sceneSizeHandle;
    DXHandle m_wMatrix;
    DXHandle m_vMatrix;
    DXHandle m_pMatrix;
};