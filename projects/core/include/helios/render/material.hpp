#pragma once

#include <helios/render/resource_manager.hpp>

namespace helios
{
    enum class ETransparencyMode
    {
        OPAQUE,
        TRANSLUCENT
    };

    enum class EShadowCaster
    {
        NONE
    };

    struct MaterialBase
    {
        ETransparencyMode transparency;
        EShadowCaster shadows;
    };

	struct StandardMaterial : MaterialBase
    {
        ResourceManager::TextureResourceHandle albedo;
        ResourceManager::TextureResourceHandle normal;
        ResourceManager::TextureResourceHandle metallic;
        ResourceManager::TextureResourceHandle occlusion;
        ResourceManager::TextureResourceHandle emission;
    };
}
