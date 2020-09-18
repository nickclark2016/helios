#pragma once

#include <helios/macros.hpp>
#include <helios/containers/bimap.hpp>
#include <helios/containers/slot_map.hpp>
#include <helios/render/texture.hpp>

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
} // namespace std

#include <functional>
#include <string>

namespace helios
{
	class ResourceManager
    {
    public:
        using TextureResourceHandle = slot_key<Texture, allocator<Texture>>;

        TextureResourceHandle addTexture(const std::string& name, IImage* image, IImageView* view, ISampler* sampler);
        Texture& getTexture(const TextureResourceHandle& handle);
        std::string getTextureName(const TextureResourceHandle& handle) const;
        void releaseTexture(const std::string& name);
        void releaseTexture(const TextureResourceHandle handle);

    private:
        slot_map<Texture> _textures;
        bimap<std::string, TextureResourceHandle> _textureNames;
        
    };
} // namespace helios
