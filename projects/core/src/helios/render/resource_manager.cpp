#include <helios/render/resource_manager.hpp>

namespace helios
{
    ResourceManager::TextureResourceHandle ResourceManager::addTexture(const std::string& name, IImage* image, IImageView* view, ISampler* sampler)
    {
        const auto handle = _textures.emplace(image, view, sampler);
        _textureNames[name] = handle;
        return handle;
    }
    
    Texture& ResourceManager::getTexture(const TextureResourceHandle& handle)
    {
        return _textures.get(handle);
    }

    std::string ResourceManager::getTextureName(
        const TextureResourceHandle& handle) const
    {
        return _textureNames[handle];
    }

    void ResourceManager::releaseTexture(
        const std::string& name)
    {
        _textures.erase(_textureNames[name]);
    }
    
    void ResourceManager::releaseTexture(const TextureResourceHandle handle)
    {
        _textures.erase(handle);
    }
} // namespace helios
