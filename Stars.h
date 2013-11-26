#pragma once
#include "engine/SceneObject.h"
#include "engine/utils.h"
#include "EngineFacade.h"
#include "SceneEffect.h"
#include <vector>

const float STAR_LAYER_POSITION = 1.5f;

class Stars : public SceneObject
{
public:
    enum
    {
        MaxStarLayers = 3
    };
    struct Star
    {
        Vec3 pos;
        Vec3 computed_pos;
        int distance;
        DWORD color;

        Star()
        {
        }
    };

    struct StarSystem
    {
        StarSystem() : vertexBuffer(0), m_count(0)
        {
            cameraPos = Vec2(0, 0);
        }

        void create(Device* device, UINT count)
        {
            m_count = count;
            device->CreateVertexBuffer(sizeof(SPRITE_VERTEX_WORLD) * count, 
                D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY | D3DUSAGE_POINTS,
                SPRITE_VERTEX_FVF, D3DPOOL_DEFAULT, &vertexBuffer, 0);
        }

        void upload()
        {
            void* v = 0;
            vertexBuffer->Lock(0, 0, &v, D3DLOCK_DISCARD);
            memcpy(v, &stars[0], sizeof(SPRITE_VERTEX_WORLD) * m_count);
            vertexBuffer->Unlock();
        }

        void update(const Vec2& pos)
        {
            Vec2 delta = pos - cameraPos;
            Matrix matrix;
            D3DXMatrixTranslation(&matrix, -delta.x / speedCoeff, -delta.y / speedCoeff, 0.0f);
            for (UINT i = 0; i < m_count; ++i)
            {
                D3DXVec3TransformCoord(&stars[i].position, &stars[i].position, &matrix);
                computeOffset(stars[i].position.x, sceneSize.x);
                computeOffset(stars[i].position.y, sceneSize.y);
            }
            cameraPos = pos;
        }

        void computeOffset(float& coord, float bound)
        {
            float diff = fabs(coord) - bound;
            if (diff > 0)
            {
                if (coord > 0.0f)
                {
                    coord = -bound + diff;
                }
                else
                {
                    coord = bound - diff;
                }
            }
        }

        void draw(Device* device, float ptSize)
        {
            device->SetRenderState(D3DRS_POINTSIZE, *(DWORD*)&ptSize);
            device->SetStreamSource(0, vertexBuffer, 0, sizeof(SPRITE_VERTEX_WORLD));
            device->SetFVF(SPRITE_VERTEX_FVF);
            device->DrawPrimitive(D3DPT_POINTLIST, 0, m_count);
        }

        void destroy()
        {
            SAFE_RELEASE(vertexBuffer);
        }
        float starSize;
        float speedCoeff;
        std::vector<SPRITE_VERTEX_WORLD> stars;
        VertexBuffer* vertexBuffer;
        UINT m_count;
        Vec2 cameraPos;
        Vec2 sceneSize;
    };

    Stars(float starsNumberPerMeter, EngineFacade& engineFacade) : m_starsNumberPerMeter(starsNumberPerMeter), 
        m_particleTexture(0), m_engineFacade(engineFacade), m_effect(0), m_regenerateStars(false)
    {
        m_starSystems.resize(MaxStarLayers);
    }

    void onCreateDevice(Device* device)
    {
        Effect* effect = m_engineFacade.effectManager.loadCachedEffect(device, "Data/render.fx");
        m_effect.reset(new SceneEffect(effect));
        m_techName = effect->GetTechniqueByName("RenderStar");
    }

    void onDestroyDevice()
    {
    }

    virtual void onLostDevice()
    {
        SAFE_RELEASE(m_particleTexture);
        for (size_t i = 0; i < m_starSystems.size(); ++i)
        {
            m_starSystems[i].destroy();
        }
    }

    virtual void onResetDevice(Device* device, const D3DSURFACE_DESC* pBackBufferSurfaceDesc)
    {
        m_device = device;
        m_particleTexture = LoadTextureFromFile(device, "Data/star.png");
        m_regenerateStars = true;
    }

    void update(float frameDelta, SceneCamera* camera)
    {
        if (m_regenerateStars)
        {
            generateStars(camera);
            m_regenerateStars = false;
        }
    }

    void draw(Scene* scene, SceneCamera* sceneCamera, Device* device)
    {
        Effect* effect = m_effect->dxEffect();

        m_effect->setIdentityMatrix(SceneEffect::WorldMatrix);
        m_effect->setIdentityMatrix(SceneEffect::ViewMatrix);
        m_effect->setProjMatrix(sceneCamera->projMatrix());

        m_effect->setBaseTexture(m_particleTexture);

        device->SetRenderState(D3DRS_POINTSPRITEENABLE, TRUE);

        effect->SetTechnique(m_techName);
        effect->Begin(0, 0);

        Matrix viewMatrix;
        D3DXMatrixIdentity(&viewMatrix);

        effect->BeginPass(0);
        //draw systems
        for (size_t i = 0; i < m_starSystems.size(); ++i)
        {
            const Vec2 cameraPos(sceneCamera->pos().x, sceneCamera->pos().y);
            m_starSystems[i].update(cameraPos);
            m_starSystems[i].upload();
            m_starSystems[i].draw(device, (FLOAT)i * 5);
        }
        effect->EndPass();

        effect->End();

        device->SetRenderState(D3DRS_POINTSPRITEENABLE, FALSE);
    }

private:
    Texture* m_particleTexture;
    std::auto_ptr<SceneEffect> m_effect;
    DXHandle m_techName;
    float m_starsNumberPerMeter;
    EngineFacade& m_engineFacade;
    bool m_regenerateStars;
    Device* m_device;

    std::vector<StarSystem> m_starSystems;

    void generateStars(SceneCamera* sceneCamera)
    {
        const size_t starCount = (size_t)(sceneCamera->viewSize().x *
                                 sceneCamera->viewSize().y *
                                 sceneCamera->maxZoom() *
                                 m_starsNumberPerMeter / MaxStarLayers);
        DWORD starColors[] = 
        {
            D3DCOLOR_XRGB(52, 129, 235),
            D3DCOLOR_XRGB(255, 234, 144),
            D3DCOLOR_XRGB(61, 82, 84),
        };

        for (size_t i = 0; i < m_starSystems.size(); ++i)
        {
            m_starSystems[i].speedCoeff = MaxStarLayers * 1.5f - i;
            m_starSystems[i].stars.resize(starCount);
            for (size_t j = 0; j < starCount; ++j)
            {
                float xHalfScene = sceneCamera->viewSize().x /*sceneCamera->maxZoom()*/ * 0.5f;
                float yHalfScene = sceneCamera->viewSize().y /*sceneCamera->maxZoom()*/ * 0.5f;

                float x = Math::randomRange(-xHalfScene, xHalfScene);
                float y = Math::randomRange(-yHalfScene, yHalfScene);

                m_starSystems[i].stars[j].position = Vec3(x, y, STAR_LAYER_POSITION * (i + 1));
                m_starSystems[i].stars[j].color = starColors[rand() % (sizeof(starColors) / sizeof(DWORD))];
                m_starSystems[i].sceneSize = Vec2(xHalfScene, yHalfScene);
            }
            m_starSystems[i].create(m_device, starCount);
            m_starSystems[i].upload();
        }
    }

};