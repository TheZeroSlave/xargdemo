#include "DXUT.h"
#include "SceneEffect.h"

SceneEffect::SceneEffect(Effect* effect) : m_effect(effect)
{
    m_textureHandle = effect->GetParameterByName(0, "g_baseTexture");

    m_wMatrix = effect->GetParameterByName(0, "g_worldMatrix");
    m_vMatrix = effect->GetParameterByName(0, "g_viewMatrix");
    m_pMatrix = effect->GetParameterByName(0, "g_projMatrix");
}

void SceneEffect::setWorldMatrix(const Matrix& worldMatrix)
{
    m_effect->SetMatrix(m_wMatrix, &worldMatrix);
}

void SceneEffect::setViewMatrix(const Matrix& viewMatrix)
{
    m_effect->SetMatrix(m_vMatrix, &viewMatrix);
}

void SceneEffect::setProjMatrix(const Matrix& projMatrix)
{
    m_effect->SetMatrix(m_pMatrix, &projMatrix);
}

void SceneEffect::setBaseTexture(Texture* texture)
{
    m_effect->SetTexture(m_textureHandle, texture);
}

void SceneEffect::applyCamera(const SceneCamera& camera)
{
    setViewMatrix(camera.viewMatrix());
    setProjMatrix(camera.projMatrix());
}