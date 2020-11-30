#include <helios/render/render_system.hpp>

#include <helios/core/engine_context.hpp>
#include <helios/core/transformation.hpp>
#include <helios/render/material.hpp>
#include <helios/render/resource_manager.hpp>

#include <entt/entt.hpp>

namespace helios
{
    extern entt::registry& get_entt(EntityManager& manager);

    RenderSystem::RenderSystem() : _manager(EngineContextFactory().create().entities())
    {
    }
    
    RenderSystem::~RenderSystem()
    {
    }
} // namespace helios