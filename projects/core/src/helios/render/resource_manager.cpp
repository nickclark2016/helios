#include <helios/render/resource_manager.hpp>

#include <helios/render/material.hpp>
#include <helios/render/render_system.hpp>

namespace helios
{
    ResourceManager::TextureResourceHandle ResourceManager::addTexture(const std::string& name, IImage* image, IImageView* view, ISampler* sampler)
    {
        const auto handle = _textures.emplace(image, view, sampler);
        _textureNames[name] = handle;
        _markTexturesDirty();
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
        _markTexturesDirty();
    }

    ResourceManager::MaterialResourceHandle ResourceManager::addMaterial(const std::string& name, Material* material)
    {
        const auto handle = _materials.emplace(material);
        _materialNames[name] = handle;
        return handle;
    }

    Material& ResourceManager::getMaterial(const MaterialResourceHandle& handle)
    {
        return *_materials.get(handle);
    }

    std::string ResourceManager::getMaterialName(const MaterialResourceHandle& handle) const
    {
        return _materialNames[handle];
    }

    void ResourceManager::releaseMaterial(const std::string& name)
    {
        delete *_materialNames[name];
        _materials.erase(_materialNames[name]);
    }

    void ResourceManager::releaseMaterial(const MaterialResourceHandle handle)
    {
        delete *handle;
        _materials.erase(handle);
    }

    bool ResourceManager::_isTexturesDirty() const noexcept
    {
        return _textureDirtyFlag;
    }

    void ResourceManager::_markTexturesDirty() noexcept
    {
        _textureDirtyFlag = true;
    }

    void ResourceManager::_resetTexturesDirty() noexcept
    {
        _textureDirtyFlag = false;
    }
} // namespace helios
