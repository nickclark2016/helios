#pragma once

#include <helios/macros.hpp>
#include <helios/math/vector.hpp>
#include <helios/render/texture.hpp>
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

    class Material
    {
    public:
        virtual ~Material() = default;
        virtual u32 serialize(u8* output, u32 start) = 0;
        virtual u32 size() const = 0;

        ETransparencyMode transparency() const noexcept;
        EShadowCaster shadows() const noexcept;

    protected:
        ETransparencyMode _transparent;
        EShadowCaster _shadowCaster;
    };

    class SimpleMaterial final : Material
    {
    public:
        virtual ~SimpleMaterial() override = default;
        u32 serialize(u8* output, u32 start) override;
        u32 size() const override;

        void albedo(ResourceManager::TextureResourceHandle albedo);
        void tiling(Vector2f tiling);
        void offset(Vector2f offset);

    private:
        ResourceManager::TextureResourceHandle _albedo;
        Vector2f _tiling;
        Vector2f _offset;
    };

    class PbrMaterial final : Material
    {
    public:
        virtual ~PbrMaterial() override = default;
        u32 serialize(u8* output, u32 start) override;
        u32 size() const override;
    
        void albedo(ResourceManager::TextureResourceHandle albedo);
        void metallic(ResourceManager::TextureResourceHandle metallic);
        void normalMap(ResourceManager::TextureResourceHandle normal);
        void occlusionMap(ResourceManager::TextureResourceHandle ao);
        void tiling(Vector2f tiling);
        void offset(Vector2f offset);

    private:
        ResourceManager::TextureResourceHandle _albedo;
        ResourceManager::TextureResourceHandle _metallic;
        ResourceManager::TextureResourceHandle _normal;
        ResourceManager::TextureResourceHandle _ao;
        Vector2f _tiling;
        Vector2f _offset;
    };
}
