#pragma once

#include "types.h"
#include "utils.h"

#include <map>
#include <string>

class EffectManager
{
public:

    EffectManager()
    {
    }

    void onCreateDevice(Device* device)
    {
    }
    
    void onDestroyDevice()
    {
        FOREACH_STD(EffectMap, m_effects)
        {
            (it->second)->Release();
        }
        m_effects.clear();
    }

    void onLostDevice()
    {
        FOREACH_STD(EffectMap, m_effects)
        {
            (it->second)->OnLostDevice();
        }
    }

    void onResetDevice()
    {
        FOREACH_STD(EffectMap, m_effects)
        {
            (it->second)->OnResetDevice();
        }
    }

    Effect* loadCachedEffect(Device* device, const char* filename)
    {
        EffectMap::iterator it = m_effects.find(filename);
        if (it != m_effects.end())
        {
            Effect* effect = it->second;
            effect->AddRef();
            return effect;
        }
        Effect* effect = LoadEffectFromFile(device, filename);
        if (effect)
        {
            m_effects.insert(std::make_pair(filename, effect));
        }

        return effect;
    }

private:
    typedef std::map<std::string, Effect*> EffectMap;
    EffectMap m_effects;
};