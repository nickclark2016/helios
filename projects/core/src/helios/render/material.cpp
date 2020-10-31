#include <helios/render/material.hpp>

#include <helios/core/serialization.hpp>

namespace helios
{
    ETransparencyMode Material::transparency() const noexcept
    {
        return _transparent;
    }

    EShadowCaster Material::shadows() const noexcept
    {
        return _shadowCaster;
    }

    u32 SimpleMaterial::serialize(u8* output, u32 start)
    {
        u32 offset = start;

        const u32 albedo = _albedo.index();

        offset = write(output, offset, albedo);
        offset = write(output, offset, _tiling);
        offset = write(output, offset, _offset);

        return offset;
    }

    u32 SimpleMaterial::size() const
    {
        u32 size = 0;

        size = align<u32>(size) + sizeof(_albedo.index());
        size = align<Vector2f>(size) + sizeof(_tiling);
        size = align<Vector2f>(size) + sizeof(_offset);

        return size;
    }

    void SimpleMaterial::albedo(ResourceManager::TextureResourceHandle albedo)
    {
        _albedo = albedo;
    }

    void SimpleMaterial::tiling(Vector2f tiling)
    {
        _tiling = tiling;
    }

    void SimpleMaterial::offset(Vector2f offset)
    {
        _offset = offset;
    }

    u32 PbrMaterial::serialize(u8* output, u32 start)
    {
        u32 offset = start;

        const u32 albedo = _albedo.index();
        const u32 metallic = _metallic.index();
        const u32 normal = _normal.index();
        const u32 ao = _ao.index();

        offset = write(output, offset, albedo);
        offset = write(output, offset, metallic);
        offset = write(output, offset, normal);
        offset = write(output, offset, ao);
        offset = write(output, offset, _tiling);
        offset = write(output, offset, _offset);

        return offset;
    }

    u32 PbrMaterial::size() const
    {
        u32 size = 0;

        size = align<u32>(size) + sizeof(_albedo.index());
        size = align<u32>(size) + sizeof(_metallic.index());
        size = align<u32>(size) + sizeof(_normal.index());
        size = align<u32>(size) + sizeof(_ao.index());
        size = align<Vector2f>(size) + sizeof(_tiling);
        size = align<Vector2f>(size) + sizeof(_offset);

        return size;
    }

    void PbrMaterial::albedo(ResourceManager::TextureResourceHandle albedo)
    {
        _albedo = albedo;
    }

    void PbrMaterial::metallic(ResourceManager::TextureResourceHandle metallic)
    {
        _metallic = metallic;
    }

    void PbrMaterial::normalMap(ResourceManager::TextureResourceHandle normal)
    {
        _normal = normal;
    }

    void PbrMaterial::occlusionMap(ResourceManager::TextureResourceHandle ao)
    {
        _ao = ao;
    }

    void PbrMaterial::tiling(Vector2f tiling)
    {
        _tiling = tiling;
    }

    void PbrMaterial::offset(Vector2f offset)
    {
        _offset = offset;
    }
} // namespace helios