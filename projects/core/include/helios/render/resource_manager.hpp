#pragma once

#include <helios/containers/bimap.hpp>
#include <helios/containers/slot_map.hpp>
#include <helios/macros.hpp>
#include <helios/render/graphics.hpp>
#include <helios/render/texture.hpp>

namespace helios
{
    class Material;
}

namespace std
{
    template <>
    struct less<helios::slot_key<helios::Texture, allocator<helios::Texture>>>
    {
        bool operator()(const helios::slot_key<helios::Texture, allocator<helios::Texture>>& lhs, 
            const helios::slot_key<helios::Texture, allocator<helios::Texture>>& rhs)
        {
            return lhs < rhs;
        }
    };

    template <>
    struct less<helios::slot_key<helios::Material, allocator<helios::Material>>>
    {
        bool operator()(const helios::slot_key<helios::Material, allocator<helios::Material>>& lhs,
                        const helios::slot_key<helios::Material, allocator<helios::Material>>& rhs)
        {
            return lhs < rhs;
        }
    };
} // namespace std

#include <functional>
#include <string>

namespace helios
{
    class RenderSystem;

	class ResourceManager
    {
    public:
        using TextureResourceHandle = slot_key<Texture, allocator<Texture>>;
        using MaterialResourceHandle = slot_key<Material*, allocator<Material*>>;

        TextureResourceHandle addTexture(const std::string& name, IImage* image, IImageView* view, ISampler* sampler);
        Texture& getTexture(const TextureResourceHandle& handle);
        std::string getTextureName(const TextureResourceHandle& handle) const;
        void releaseTexture(const std::string& name);
        void releaseTexture(const TextureResourceHandle handle);

        MaterialResourceHandle addMaterial(const std::string& name, Material* material);
        Material& getMaterial(const MaterialResourceHandle& handle);
        std::string getMaterialName(const MaterialResourceHandle& handle) const;
        void releaseMaterial(const std::string& name);
        void releaseMaterial(const MaterialResourceHandle handle);

    private:
        friend class RenderSystem;

        slot_map<Texture> _textures;
        bimap<std::string, TextureResourceHandle> _textureNames;
        slot_map<Material*> _materials;
        bimap<std::string, MaterialResourceHandle> _materialNames;
        
        bool _isTexturesDirty() const noexcept;
        void _markTexturesDirty() noexcept;
        void _resetTexturesDirty() noexcept;
        bool _textureDirtyFlag = false;
    };
} // namespace helios
