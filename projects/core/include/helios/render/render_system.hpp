#pragma once

#include <helios/macros.hpp>
#include <helios/containers/vector.hpp>
#include <helios/ecs/entity.hpp>
#include <helios/math/matrix.hpp>
#include <helios/render/graphics.hpp>

namespace helios
{
	class RenderSystem final
    {
    public:
        RenderSystem();
        ~RenderSystem();

        HELIOS_NO_COPY_MOVE(RenderSystem)

    private:

        EntityManager& _manager;
    };
}