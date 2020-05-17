#include <helios/containers/memory.hpp>

void* operator new(size_t sz)
{
    return helios::mem_alloc(sz, helios::EMemoryTag::TAG_NEW);
}

void operator delete(void* ptr) noexcept
{
    helios::mem_free(ptr);
}

void* operator new[](size_t sz)
{
    return helios::mem_alloc(sz, helios::EMemoryTag::TAG_NEW);
}

void operator delete[](void* ptr) noexcept
{
    helios::mem_free(ptr);
}

void* operator new(size_t sz, helios::EMemoryTag tag)
{
    return helios::mem_alloc(sz, tag);
}

void operator delete(void* ptr, helios::EMemoryTag tag) noexcept
{
    helios::mem_free(ptr);
}

void* operator new[](size_t sz, helios::EMemoryTag tag)
{
    return helios::mem_alloc(sz, tag);
}

void operator delete[](void* ptr, helios::EMemoryTag tag) noexcept
{
    helios::mem_free(ptr);
}

namespace helios
{
    void* mem_alloc_align_16(size_t sz, EMemoryTag tag)
    {
#if defined(_WIN32) || defined(__CYGWIN__)
        return _aligned_alloc(16, sz);
#else
        return aligned_alloc(16, sz);
#endif
    }

    void mem_free_align_16(void* ptr)
    {
        free(ptr);
    }

    void* mem_alloc(size_t sz, EMemoryTag tag)
    {
        return malloc(sz);
    }

    void mem_free(void* ptr)
    {
        return free(ptr);
    }

    void* mem_clear_alloc(size_t sz, EMemoryTag tag)
    {
        return calloc(sz, 1);
    }
} // namespace helios