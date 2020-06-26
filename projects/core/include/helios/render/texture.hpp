#pragma once

#include <helios/macros.hpp>
#include <helios/render/graphics.hpp>

namespace helios
{
    class Texture final
    {
    public:
        Texture(IImage* image, IImageView* view, ISampler* sampler);
        HELIOS_NO_COPY_MOVE(Texture)
        ~Texture() = default;

        IImage* getImage() const noexcept;
        IImageView* getImageView() const noexcept;
        ISampler* getSampler() const noexcept;

    private:
        IImage* _image;
        IImageView* _view;
        ISampler* _sampler;
    };
}; // namespace helios
