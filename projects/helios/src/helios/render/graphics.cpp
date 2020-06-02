#include <helios/render/graphics.hpp>

#if defined(_WIN32)
#pragma warning(push)
#pragma warning(disable: 26495)
#endif

namespace helios
{
    DescriptorWriteInfo::DescriptorWriteInfo()
    {
        memset(this, 0, sizeof(std::remove_pointer_t<decltype(this)>));
    }

    DescriptorWriteInfo::DescriptorWriteInfo(const DescriptorWriteInfo& other)
        : binding(other.binding), element(other.element), type(other.type)
    {
        switch (type)
        {
        case EDescriptorType::UNDEFINED: break;
        case EDescriptorType::SAMPLER:
        case EDescriptorType::COMBINED_IMAGE_SAMPLER:
        case EDescriptorType::SAMPLED_IMAGE:
        case EDescriptorType::STORAGE_IMAGE:
        case EDescriptorType::INPUT_ATTACHMENT: {
			images = other.images;
            break;
        }
        case EDescriptorType::UNIFORM_TEXEL_BUFFER:
        case EDescriptorType::STORAGE_TEXEL_BUFFER: break;
        case EDescriptorType::UNIFORM_BUFFER:
        case EDescriptorType::STORAGE_BUFFER:
        case EDescriptorType::UNIFORM_BUFFER_DYNAMIC:
        case EDescriptorType::STORAGE_BUFFER_DYNAMIC: {
            buffers = other.buffers;
            break;
        }
        default: ;
        }
    }

    DescriptorWriteInfo::DescriptorWriteInfo(
        DescriptorWriteInfo&& other) noexcept
    {
        switch (type)
        {
        case EDescriptorType::UNDEFINED:
            break;
        case EDescriptorType::SAMPLER:
        case EDescriptorType::COMBINED_IMAGE_SAMPLER:
        case EDescriptorType::SAMPLED_IMAGE:
        case EDescriptorType::STORAGE_IMAGE:
        case EDescriptorType::INPUT_ATTACHMENT: {
            images = helios::move(other.images);
            break;
        }
        case EDescriptorType::UNIFORM_TEXEL_BUFFER:
        case EDescriptorType::STORAGE_TEXEL_BUFFER:
            break;
        case EDescriptorType::UNIFORM_BUFFER:
        case EDescriptorType::STORAGE_BUFFER:
        case EDescriptorType::UNIFORM_BUFFER_DYNAMIC:
        case EDescriptorType::STORAGE_BUFFER_DYNAMIC: {
            buffers = helios::move(other.buffers);
            break;
        }
        default:;
        }
    }

    DescriptorWriteInfo::~DescriptorWriteInfo()
    {
        switch (type)
        {
        case EDescriptorType::UNDEFINED:
            break;
        case EDescriptorType::SAMPLER:
        case EDescriptorType::COMBINED_IMAGE_SAMPLER:
        case EDescriptorType::SAMPLED_IMAGE:
        case EDescriptorType::STORAGE_IMAGE:
        case EDescriptorType::INPUT_ATTACHMENT: {
            images.clear();
            break;
        }
        case EDescriptorType::UNIFORM_TEXEL_BUFFER:
        case EDescriptorType::STORAGE_TEXEL_BUFFER:
            break;
        case EDescriptorType::UNIFORM_BUFFER:
        case EDescriptorType::STORAGE_BUFFER:
        case EDescriptorType::UNIFORM_BUFFER_DYNAMIC:
        case EDescriptorType::STORAGE_BUFFER_DYNAMIC: {
            buffers.clear();
            break;
        }
        default:;
        }
    }

    DescriptorWriteInfo& DescriptorWriteInfo::operator=(
        const DescriptorWriteInfo& other)
    {
        switch (type)
        {
        case EDescriptorType::UNDEFINED:
            break;
        case EDescriptorType::SAMPLER:
        case EDescriptorType::COMBINED_IMAGE_SAMPLER:
        case EDescriptorType::SAMPLED_IMAGE:
        case EDescriptorType::STORAGE_IMAGE:
        case EDescriptorType::INPUT_ATTACHMENT: {
            images.clear();
            break;
        }
        case EDescriptorType::UNIFORM_TEXEL_BUFFER:
        case EDescriptorType::STORAGE_TEXEL_BUFFER:
            break;
        case EDescriptorType::UNIFORM_BUFFER:
        case EDescriptorType::STORAGE_BUFFER:
        case EDescriptorType::UNIFORM_BUFFER_DYNAMIC:
        case EDescriptorType::STORAGE_BUFFER_DYNAMIC: {
            buffers.clear();
            break;
        }
        default:;
        }

    	binding = other.binding;
        element = other.element;
    	type = other.type;
    	
        switch (type)
        {
        case EDescriptorType::UNDEFINED:
            break;
        case EDescriptorType::SAMPLER:
        case EDescriptorType::COMBINED_IMAGE_SAMPLER:
        case EDescriptorType::SAMPLED_IMAGE:
        case EDescriptorType::STORAGE_IMAGE:
        case EDescriptorType::INPUT_ATTACHMENT: {
            images = other.images;
            break;
        }
        case EDescriptorType::UNIFORM_TEXEL_BUFFER:
        case EDescriptorType::STORAGE_TEXEL_BUFFER:
            break;
        case EDescriptorType::UNIFORM_BUFFER:
        case EDescriptorType::STORAGE_BUFFER:
        case EDescriptorType::UNIFORM_BUFFER_DYNAMIC:
        case EDescriptorType::STORAGE_BUFFER_DYNAMIC: {
            buffers = other.buffers;
            break;
        }
        default:;
        }

    	return *this;
    }

    DescriptorWriteInfo& DescriptorWriteInfo::operator=(
        DescriptorWriteInfo&& other) noexcept
    {
        switch (type)
        {
        case EDescriptorType::UNDEFINED:
            break;
        case EDescriptorType::SAMPLER:
        case EDescriptorType::COMBINED_IMAGE_SAMPLER:
        case EDescriptorType::SAMPLED_IMAGE:
        case EDescriptorType::STORAGE_IMAGE:
        case EDescriptorType::INPUT_ATTACHMENT: {
            images.clear();
            break;
        }
        case EDescriptorType::UNIFORM_TEXEL_BUFFER:
        case EDescriptorType::STORAGE_TEXEL_BUFFER:
            break;
        case EDescriptorType::UNIFORM_BUFFER:
        case EDescriptorType::STORAGE_BUFFER:
        case EDescriptorType::UNIFORM_BUFFER_DYNAMIC:
        case EDescriptorType::STORAGE_BUFFER_DYNAMIC: {
            buffers.clear();
            break;
        }
        default:;
        }

        binding = other.binding;
        element = other.element;
        type = other.type;

        switch (type)
        {
        case EDescriptorType::UNDEFINED:
            break;
        case EDescriptorType::SAMPLER:
        case EDescriptorType::COMBINED_IMAGE_SAMPLER:
        case EDescriptorType::SAMPLED_IMAGE:
        case EDescriptorType::STORAGE_IMAGE:
        case EDescriptorType::INPUT_ATTACHMENT: {
            images = helios::move(other.images);
            break;
        }
        case EDescriptorType::UNIFORM_TEXEL_BUFFER:
        case EDescriptorType::STORAGE_TEXEL_BUFFER:
            break;
        case EDescriptorType::UNIFORM_BUFFER:
        case EDescriptorType::STORAGE_BUFFER:
        case EDescriptorType::UNIFORM_BUFFER_DYNAMIC:
        case EDescriptorType::STORAGE_BUFFER_DYNAMIC: {
            buffers = helios::move(other.buffers);
            break;
        }
        default:;
        }

    	return *this;
    }
} // namespace helios

#if defined(_WIN32)
#pragma warning(pop)
#endif