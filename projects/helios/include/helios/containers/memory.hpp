#pragma once

#include <helios/macros.hpp>
#include <helios/type_traits.hpp>
#include <helios/utility.hpp>

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <new>

namespace helios
{
    enum class EMemoryTag : uint32_t
    {
        TAG_DEBUG,
        TAG_NEW,
        TAG_BLOCK,
        TAG_VERTS,
        TAG_INDEXES,
        TAG_PAGE,
        TAG_DEFRAG_BLOCK
    };

    void* mem_alloc_align_16(size_t sz, EMemoryTag tag);
    void mem_free_align_16(void* ptr);
    void* mem_alloc(size_t sz, EMemoryTag tag);
    void mem_free(void* ptr);
    void* mem_clear_alloc(size_t sz, EMemoryTag tag);
} // namespace helios

HELIOS_NO_DISCARD void* operator new(size_t sz);
void operator delete(void* ptr) noexcept;
HELIOS_NO_DISCARD void* operator new[](size_t sz);
void operator delete[](void* ptr) noexcept;
HELIOS_NO_DISCARD void* operator new(size_t sz, helios::EMemoryTag tag);
void operator delete(void* ptr, helios::EMemoryTag tag) noexcept;
HELIOS_NO_DISCARD void* operator new[](size_t sz, helios::EMemoryTag tag);
void operator delete[](void* ptr, helios::EMemoryTag tag) noexcept;

namespace helios
{
    template <typename T>
    class allocator
    {
    public:
        virtual T* allocate(size_t count);
        virtual void release(T* ptr);
    };

    template <typename T>
    inline T* allocator<T>::allocate(size_t count)
    {
        if constexpr (is_same_v<T, void>)
        {
            return malloc(count);
        }
        return reinterpret_cast<T*>(malloc(sizeof(T) * count));
    }

    template <typename T>
    inline void allocator<T>::release(T* ptr)
    {
        free(ptr);
    }

    namespace detail
    {
        template <typename Type>
        struct ControlBlock
        {
            Type* ptr;
            size_t count;
        };
    } // namespace detail

    using nullptr_t = decltype(nullptr);

    template <typename Type>
    class shared_ptr
    {
    public:
        shared_ptr();
        shared_ptr(nullptr_t);
        shared_ptr(Type* ptr);
        shared_ptr(const shared_ptr& ptr);
        shared_ptr(shared_ptr&& ptr) noexcept;

        template <typename Other>
        shared_ptr(const shared_ptr<Other>& ptr);

        template <typename Other>
        shared_ptr(shared_ptr<Other>&& ptr);

        ~shared_ptr();

        shared_ptr& operator=(const shared_ptr& ptr);
        shared_ptr& operator=(shared_ptr&& ptr) noexcept;
        shared_ptr& operator=(nullptr_t);

        template <typename Other>
        shared_ptr& operator=(const shared_ptr<Other>& ptr);

        template <typename Other>
        shared_ptr& operator=(shared_ptr<Other>&& ptr);

        void reset() noexcept;

        template <class Other>
        void reset(Other* ptr);

        void swap(shared_ptr& ptr) noexcept;

        [[nodiscard]] Type* get() const noexcept;

        [[nodiscard]] Type* operator->() const noexcept;
        [[nodiscard]] Type& operator*() const noexcept;

        [[nodiscard]] size_t use_count() const noexcept;
        [[nodiscard]] bool unique() const noexcept;
        explicit operator bool() const noexcept;

        [[nodiscard]] bool operator==(const shared_ptr& other) const noexcept;
        [[nodiscard]] bool operator!=(const shared_ptr& other) const noexcept;

        template <typename Other>
        [[nodiscard]] bool operator==(
            const shared_ptr<Other>& other) const noexcept;

        template <typename Other>
        [[nodiscard]] bool operator!=(
            const shared_ptr<Other>& other) const noexcept;

        template <typename Output>
        inline friend shared_ptr<Output> dynamic_pointer_cast(
            const shared_ptr& ptr)
        {
            return ptr.__dynamic_pointer_cast<Output>();
        }

        template <typename Output>
        inline friend shared_ptr<Output> reinterpret_pointer_cast(
            const shared_ptr& ptr)
        {
            return ptr.__reinterpret_pointer_cast<Output>();
        }

    private:
        template <typename Output>
        [[nodiscard]] inline shared_ptr<Output> __dynamic_pointer_cast() const
        {
            if (dynamic_cast<Output*>(get()))
            {
                shared_ptr<Output> res;
                res._blk =
                    reinterpret_cast<detail::ControlBlock<Output>*>(_blk);
                ++_blk->count;
                return res;
            }
            return nullptr;
        }

        template <typename Output>
        inline shared_ptr<Output> __reinterpret_pointer_cast() const
        {
            shared_ptr<Output> res;
            res._blk = reinterpret_cast<detail::ControlBlock<Output>*>(_blk);
            if (_blk)
            {
                ++_blk->count;
            }
            return res;
        }

    private:
        template <typename Other>
        friend class shared_ptr;

        detail::ControlBlock<Type>* _blk;
    };

    template <typename Type>
    inline shared_ptr<Type>::shared_ptr() : shared_ptr(nullptr)
    {
    }

    template <typename Type>
    inline shared_ptr<Type>::shared_ptr(nullptr_t) : _blk(nullptr)
    {
    }

    template <typename Type>
    inline shared_ptr<Type>::shared_ptr(Type* ptr) : shared_ptr()
    {
        if (ptr)
        {
            _blk = new detail::ControlBlock<Type>;
            _blk->count = 1;
            _blk->ptr = ptr;
        }
    }

    template <typename Type>
    inline shared_ptr<Type>::shared_ptr(const shared_ptr& ptr) : _blk(ptr._blk)
    {
        if (_blk)
        {
            ++_blk->count;
        }
    }

    template <typename Type>
    inline shared_ptr<Type>::shared_ptr(shared_ptr&& ptr) noexcept
        : _blk(ptr._blk)
    {
        ptr._blk = nullptr;
    }

    template <typename Type>
    template <typename Other>
    inline shared_ptr<Type>::shared_ptr(const shared_ptr<Other>& ptr)
        : _blk(nullptr)
    {
        static_assert(is_base_of_v<Type, Other> || is_base_of_v<Other, Type> ||
                          is_same_v<Other, Type> || is_same_v<Other, void>,
                      "Cannot convert between types.");
        _blk = reinterpret_cast<detail::ControlBlock<Type>*>(ptr._blk);
        if (_blk)
        {
            ++_blk->count;
        }
    }

    template <typename Type>
    template <typename Other>
    inline shared_ptr<Type>::shared_ptr(shared_ptr<Other>&& ptr) : _blk(nullptr)
    {
        static_assert(is_base_of_v<Type, Other> || is_base_of_v<Other, Type> ||
                          is_same_v<Other, Type> || is_same_v<Other, void>,
                      "Cannot convert between types.");
        _blk = reinterpret_cast<detail::ControlBlock<Type>*>(ptr._blk);
        ptr._blk = nullptr;
    }

    template <typename Type>
    inline shared_ptr<Type>::~shared_ptr()
    {
        if (_blk)
        {
            if (_blk->count == 0)
            {
                delete _blk;
                _blk = nullptr;
            }
            else
            {
                --_blk->count;
            }
        }
    }

    template <typename Type>
    inline shared_ptr<Type>& shared_ptr<Type>::operator=(const shared_ptr& ptr)
    {
        if (reinterpret_cast<void*>(this) !=
                reinterpret_cast<const void*>(&ptr) &&
            _blk != ptr._blk)
        {
            _blk = ptr._blk;
            if (_blk)
            {
                ++_blk->count;
            }
        }
        return *this;
    }

    template <typename Type>
    inline shared_ptr<Type>& shared_ptr<Type>::operator=(
        shared_ptr&& ptr) noexcept
    {
        if (reinterpret_cast<void*>(this) != reinterpret_cast<void*>(&ptr))
        {
            _blk = ptr._blk;
            ptr._blk = nullptr;
        }
        return *this;
    }

    template <typename Type>
    inline shared_ptr<Type>& shared_ptr<Type>::operator=(nullptr_t)
    {
        if (_blk)
        {
            --_blk->count;
            if (_blk->count == 0)
            {
                delete _blk;
            }
        }
        _blk = nullptr;
        return *this;
    }

    template <typename Type>
    template <typename Other>
    inline shared_ptr<Type>& shared_ptr<Type>::operator=(
        const shared_ptr<Other>& ptr)
    {
        static_assert(is_base_of_v<Type, Other> || is_base_of_v<Other, Type> ||
                          is_same_v<Other, Type> || is_same_v<Other, void>,
                      "Cannot convert between types.");
        if (reinterpret_cast<void*>(this) !=
                reinterpret_cast<const void*>(&ptr) &&
            reinterpret_cast<void*>(_blk) !=
                reinterpret_cast<const void*>(ptr._blk))
        {
            _blk = reinterpret_cast<detail::ControlBlock<Type>*>(ptr._blk);
            if (_blk)
            {
                ++_blk->count;
            }
        }
        return *this;
    }

    template <typename Type>
    template <typename Other>
    inline shared_ptr<Type>& shared_ptr<Type>::operator=(
        shared_ptr<Other>&& ptr)
    {
        static_assert(is_base_of_v<Type, Other> || is_base_of_v<Other, Type> ||
                          is_same_v<Other, Type> || is_same_v<Other, void>,
                      "Cannot convert between types.");
        if (reinterpret_cast<void*>(this) != reinterpret_cast<void*>(&ptr))
        {
            _blk = reinterpret_cast<detail::ControlBlock<Type>*>(ptr._blk);
            ptr._blk = nullptr;
        }
        return *this;
    }

    template <typename Type>
    inline void shared_ptr<Type>::reset() noexcept
    {
        if (_blk)
        {
            if (unique())
            {
                delete _blk;
            }
            else
            {
                --_blk->count;
            }
        }
        _blk = nullptr;
    }

    template <typename Type>
    template <class Other>
    inline void shared_ptr<Type>::reset(Other* ptr)
    {
        static_assert(is_base_of_v<Type, Other> || is_base_of_v<Other, Type> ||
                          is_same_v<Other, Type> || is_same_v<Other, void>,
                      "Cannot convert between types.");
        if (_blk)
        {
            if (unique())
            {
                _blk->ptr = reinterpret_cast<Type*>(ptr);
            }
            else
            {
                --_blk->count;
                _blk = new detail::ControlBlock<Type>;
                _blk->count = 1;
                _blk->ptr = reinterpret_cast<Type*>(ptr);
            }
        }
        else
        {
            _blk = new detail::ControlBlock<Type>;
            _blk->count = 1;
            _blk->ptr = reinterpret_cast<Type*>(ptr);
        }
    }

    template <typename Type>
    inline void shared_ptr<Type>::swap(shared_ptr& ptr) noexcept
    {
        const auto blk = ptr._blk;
        ptr._blk = _blk;
        _blk = blk;
    }

    template <typename Type>
    inline Type* shared_ptr<Type>::get() const noexcept
    {
        if (_blk)
        {
            return _blk->ptr;
        }
        return nullptr;
    }

    template <typename Type>
    inline Type* shared_ptr<Type>::operator->() const noexcept
    {
        return get();
    }

    template <typename Type>
    inline Type& shared_ptr<Type>::operator*() const noexcept
    {
        return *get();
    }

    template <typename Type>
    inline size_t shared_ptr<Type>::use_count() const noexcept
    {
        if (_blk)
        {
            return _blk->count;
        }
        return 0;
    }

    template <typename Type>
    inline bool shared_ptr<Type>::unique() const noexcept
    {
        return use_count() == 1;
    }

    template <typename Type>
    inline shared_ptr<Type>::operator bool() const noexcept
    {
        return use_count() > 0;
    }

    template <typename Type>
    inline bool shared_ptr<Type>::operator==(
        const shared_ptr& other) const noexcept
    {
        return _blk == other._blk;
    }

    template <typename Type>
    bool shared_ptr<Type>::operator!=(const shared_ptr& other) const noexcept
    {
        return _blk != other._blk;
    }

    template <typename Type>
    template <typename Other>
    bool shared_ptr<Type>::operator==(
        const shared_ptr<Other>& other) const noexcept
    {
        return _blk == other._blk;
    }

    template <typename Type>
    template <typename Other>
    bool shared_ptr<Type>::operator!=(
        const shared_ptr<Other>& other) const noexcept
    {
        return _blk != other._blk;
    }

    template <typename Type, typename... Args>
    inline shared_ptr<Type> make_shared(Args&&... args)
    {
        Type* t = ::new Type(forward<Args>(args)...);
        return shared_ptr<Type>(t);
    }
} // namespace helios
