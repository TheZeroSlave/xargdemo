#pragma once

#include "DeviceObject.h"
#include "EmptyScene.h"
#include "SceneCamera.h"
#include "types.h"

class SceneObject : public DeviceObject
{
public:
    SceneObject()
    {
    }
    virtual void update(float frameDelta, SceneCamera* sceneCamera){}
    virtual void draw(Scene* scene, SceneCamera* sceneCamera, Device* device) = 0;
protected:
    Scene* m_scene;
};