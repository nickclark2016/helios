#pragma once

#include <helios/macros.hpp>

namespace helios
{
    enum class EMemoryTag : u32
    {
        TAG_DEBUG,
        TAG_NEW,
        TAG_BLOCK,
        VERTEX_BUFFER,
        INDEX_BUFFER,
        UNIFORM_BUFFER,
        SHADER_STORAGE_BUFFER,
        TEXTURE_BUFFER,
        ECS_COMPONENT_MAP
    };
}
