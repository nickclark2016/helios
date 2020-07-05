#include <helios/render/texture.hpp>

namespace helios
{
    Texture::Texture(IImage* image, IImageView* view, ISampler* sampler) : _image(image), _view(view), _sampler(sampler)
    {
    }

    IImage* Texture::getImage() const noexcept
    {
        return _image;
    }

    IImageView* Texture::getImageView() const noexcept
    {
        return _view;
    }

    ISampler* Texture::getSampler() const noexcept
    {
        return _sampler;
    }
} // namespace helios
