#pragma once
#include "engine/types.h"
#include "engine/SceneObject.h"

class SpaceObject : public SceneObject
{
public:
    float mass;
    Vec2  velocity;
    Vec2  position;

    SpaceObject(const Vec2& initialPos) : mass(1.0f), position(initialPos)
    {
        velocity = Vec2(0, 0);
        m_force = Vec2(0, 0);
    }

    void applyForce(const Vec2& force)
    {
        m_force += force;
    }

    virtual void update(float deltaFrame, SceneCamera* sceneCamera)
    {
        compute(deltaFrame);
        m_force = Vec2(0, 0);
    }

private:
    Vec2 m_force;

    void compute(float dt)
    {
        Vec2 a = m_force / mass;

        velocity += a * dt;
        position += velocity * dt;
    }
};