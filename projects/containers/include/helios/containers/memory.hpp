#pragma once

#include <helios/containers/memory_tags.hpp>
#include <helios/containers/tree.hpp>
#include <helios/containers/type_traits.hpp>
#include <helios/containers/utility.hpp>
#include <helios/macros.hpp>

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <new>

#ifndef HELIOS_BLOCK_ALLOCATOR_ALIGNMENT
#define HELIOS_BLOCK_ALLOCATOR_ALIGNMENT 16
#endif

namespace helios
{
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
        T* allocate(size_t count);
        void release(T* ptr);
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

    template <typename T, u32 BlockSize, EMemoryTag tag = EMemoryTag::TAG_BLOCK>
    class block_allocator
    {
    public:
        block_allocator();
        block_allocator(bool clear);
        block_allocator(const block_allocator&) = delete;
        block_allocator(block_allocator&& other) noexcept;
        ~block_allocator();

        block_allocator& operator=(const block_allocator&) = delete;
        block_allocator& operator=(block_allocator&& other) noexcept;

        T* allocate();
        void release(T* ptr);
        void release_empty_blocks();
        void release_all();

        size_t allocation_count() const;

    private:
        union element
        {
            T* data;
            element* next;
            u8 buffer[(max(sizeof(T), sizeof(element*)) + (HELIOS_BLOCK_ALLOCATOR_ALIGNMENT - 1)) &
                      ~(HELIOS_BLOCK_ALLOCATOR_ALIGNMENT - 1)];
        };

        struct block
        {
            element elements[BlockSize];
            block* next;
            element* free_list;
            u32 free_count;
        };

        block* _mem_blocks;
        element* _free_elements;
        size_t _total_elements;
        size_t _active_elements;
        bool _clear_allocations;

        block* _create_new_block();
    };

    template <typename T, u32 BlockSize, EMemoryTag Tag>
    inline block_allocator<T, BlockSize, Tag>::block_allocator() : block_allocator(false)
    {
    }

    template <typename T, u32 BlockSize, EMemoryTag Tag>
    inline block_allocator<T, BlockSize, Tag>::block_allocator(bool clear)
        : _mem_blocks(nullptr), _free_elements(nullptr), _total_elements(0), _active_elements(0),
          _clear_allocations(clear)
    {
    }

    template <typename T, u32 BlockSize, EMemoryTag Tag>
    inline block_allocator<T, BlockSize, Tag>::block_allocator(block_allocator&& other) noexcept
        : _mem_blocks(other._mem_blocks), _free_elements(other._free_elements), _total_elements(other._total_elements),
          _active_elements(other._active_elements), _clear_allocations(other._clear_allocations)
    {
        other._mem_blocks = nullptr;
        other._free_elements = nullptr;
        other._total_elements = 0;
        other._active_elements = 0;
        other._clear_allocations = false;
    }

    template <typename T, u32 BlockSize, EMemoryTag Tag>
    inline block_allocator<T, BlockSize, Tag>::~block_allocator()
    {
        release_all();
    }

    template <typename T, u32 BlockSize, EMemoryTag Tag>
    inline block_allocator<T, BlockSize, Tag>& block_allocator<T, BlockSize, Tag>::operator=(
        block_allocator&& other) noexcept
    {
        _mem_blocks = other._mem_blocks;
        _free_elements = other._free_elements;
        _total_elements = other._total_elements;
        _active_elements = other._active_elements;
        _clear_allocations = other._clear_allocations;

        other._mem_blocks = nullptr;
        other._free_elements = nullptr;
        other._total_elements = 0;
        other._active_elements = 0;
        other._clear_allocations = false;

        return *this;
    }

    template <typename T, u32 BlockSize, EMemoryTag Tag>
    inline T* block_allocator<T, BlockSize, Tag>::allocate()
    {
        // check if there are no empty blocks
        if (_free_elements == nullptr)
        {
            _create_new_block();
        }
        ++_active_elements;
        element* elem = _free_elements;
        _free_elements = _free_elements->next;
        elem->next = nullptr;

        T* ptr = reinterpret_cast<T*>(elem->buffer);
        if (_clear_allocations)
        {
            memset(ptr, 0, sizeof(T));
        }
        return ptr;
    }

    template <typename T, u32 BlockSize, EMemoryTag Tag>
    inline void block_allocator<T, BlockSize, Tag>::release(T* ptr)
    {
        if (ptr != nullptr)
        {
            element* elem = reinterpret_cast<element*>(ptr);
            elem->next = _free_elements;
            _free_elements = elem;
            --_active_elements;
        }
    }

    template <typename T, u32 BlockSize, EMemoryTag Tag>
    inline void block_allocator<T, BlockSize, Tag>::release_empty_blocks()
    {
        // build the free chain for each block of memory
        block* blk = _mem_blocks;
        while (blk != nullptr)
        {
            blk->free_list = nullptr;
            blk->free_count = 0;
            // next block
            blk = blk->next;
        }

        element* elem = _free_elements;
        while (elem != nullptr)
        {
            element* next = elem->next;
            blk = _mem_blocks;
            while (blk != nullptr)
            {
                // check to make sure the element is in the block
                if (elem >= blk->elements && elem < blk->elements + BlockSize)
                {
                    elem->next = blk->free_list;
                    blk->free_list = elem;
                    ++(blk->free_count);
                    break;
                }
                blk = blk->next;
            }
            elem = next;
        }

        // Free all blocks where the free_count == BlockSize
        block* previous = nullptr;
        blk = _mem_blocks;
        while (blk != nullptr)
        {
            block* next = blk->next;
            // check if the block is empty
            if (blk->free_count == BlockSize)
            {
                if (previous == nullptr)
                {
                    _mem_blocks = next;
                }
                else
                {
                    previous->next = next;
                }
                mem_free(blk);
                _total_elements -= BlockSize;
            }
            else
            {
                previous = blk;
            }
            blk = next;
        }

        // Rebuild the free chain
        _free_elements = nullptr;
        blk = _mem_blocks;
        while (blk != nullptr)
        {
            element* elem = blk->free_list;
            while (elem != nullptr)
            {
                element* next = elem->next;
                elem->next = _free_elements;
                _free_elements = elem;
                elem = next;
            }
        }
    }

    template <typename T, u32 BlockSize, EMemoryTag Tag>
    inline void block_allocator<T, BlockSize, Tag>::release_all()
    {
        while (_mem_blocks != nullptr)
        {
            block* blk = _mem_blocks;
            _mem_blocks = _mem_blocks->next;
            mem_free(blk);
        }
        _total_elements = _active_elements = 0;
    }

    template <typename T, u32 BlockSize, EMemoryTag Tag>
    inline size_t block_allocator<T, BlockSize, Tag>::allocation_count() const
    {
        return _active_elements;
    }

    template <typename T, u32 BlockSize, EMemoryTag Tag>
    inline typename block_allocator<T, BlockSize, Tag>::block* block_allocator<T, BlockSize, Tag>::_create_new_block()
    {
        block* blk = reinterpret_cast<block*>(mem_alloc(sizeof(block), Tag));
        blk->next = _mem_blocks;
        _mem_blocks = blk;
        for (u32 i = 0; i < BlockSize; i++)
        {
            blk->elements[i].next = _free_elements;
            _free_elements = blk->elements + i;
        }
        _total_elements += BlockSize;

        return blk;
    }

    template <typename Type, size_t MinBlockSize, size_t MinimumByteCount, EMemoryTag Tag = EMemoryTag::TAG_BLOCK>
    class dynamic_block_allocator
    {
    public:
        dynamic_block_allocator();
        dynamic_block_allocator(const dynamic_block_allocator&) = delete;
        dynamic_block_allocator(dynamic_block_allocator&& other) noexcept;
        ~dynamic_block_allocator();
        dynamic_block_allocator& operator=(const dynamic_block_allocator&) = delete;
        dynamic_block_allocator& operator=(dynamic_block_allocator&& other) noexcept;

        Type* allocate(size_t count);
        void release(Type* ptr);
        void release_all();

    private:
        struct dynamic_block;

        using tree_type = btree<size_t, dynamic_block, 4>;
        struct dynamic_block
        {
            static constexpr size_t BaseMask = size_t(1) << 63;

            Type* get_ptr() const;
            size_t size() const noexcept;
            void size(size_t size, bool base);
            bool is_base_block() const noexcept;

            size_t sz;
            dynamic_block* prev;
            dynamic_block* next;
            typename tree_type::node_t* node;
            void* allocator;
        };

        dynamic_block* _firstBlock;
        dynamic_block* _finalBlock;
        tree_type _freeTree;

        void _initialize();
        void _finalize();
        void _attach_freed_node(dynamic_block* blk);
        void _detach_freed_node(dynamic_block* blk);
        dynamic_block* _alloc_block(size_t num);
        dynamic_block* _resize_block(dynamic_block* blk, size_t num);
        void _release_block(dynamic_block* blk);
    };

    template <typename Type, size_t MinBlockSize, size_t MinimumByteCount, EMemoryTag Tag>
    inline Type* dynamic_block_allocator<Type, MinBlockSize, MinimumByteCount, Tag>::dynamic_block::get_ptr() const
    {
        // payload occurs after the
        const u8* me = reinterpret_cast<const u8*>(this);
        const u8* payload = me + sizeof(dynamic_block);
        return const_cast<Type*>(reinterpret_cast<const Type*>(payload));
    }

    template <typename Type, size_t MinBlockSize, size_t MinimumByteCount, EMemoryTag Tag>
    inline size_t dynamic_block_allocator<Type, MinBlockSize, MinimumByteCount, Tag>::dynamic_block::size()
        const noexcept
    {
        return sz & ~BaseMask;
    }

    template <typename Type, size_t MinBlockSize, size_t MinimumByteCount, EMemoryTag Tag>
    inline void dynamic_block_allocator<Type, MinBlockSize, MinimumByteCount, Tag>::dynamic_block::size(size_t size,
                                                                                                        bool base)
    {
        sz = size;
        if (base)
        {
            sz |= BaseMask;
        }
    }

    template <typename Type, size_t MinBlockSize, size_t MinimumByteCount, EMemoryTag Tag>
    inline bool dynamic_block_allocator<Type, MinBlockSize, MinimumByteCount, Tag>::dynamic_block::is_base_block()
        const noexcept
    {
        return (sz & BaseMask) != 0;
    }

    template <typename Type, size_t MinBlockSize, size_t MinimumByteCount, EMemoryTag Tag>
    inline dynamic_block_allocator<Type, MinBlockSize, MinimumByteCount, Tag>::dynamic_block_allocator()
    {
        _initialize();
    }

    template <typename Type, size_t MinBlockSize, size_t MinimumByteCount, EMemoryTag Tag>
    inline dynamic_block_allocator<Type, MinBlockSize, MinimumByteCount, Tag>::dynamic_block_allocator(
        dynamic_block_allocator&& other) noexcept
        : _firstBlock(other._firstBlock), _finalBlock(other._finalBlock), _freeTree(helios::move(other._freeTree))
    {
    }

    template <typename Type, size_t MinBlockSize, size_t MinimumByteCount, EMemoryTag Tag>
    inline dynamic_block_allocator<Type, MinBlockSize, MinimumByteCount, Tag>::~dynamic_block_allocator()
    {
        _finalize();
    }

    template <typename Type, size_t MinBlockSize, size_t MinimumByteCount, EMemoryTag Tag>
    inline dynamic_block_allocator<Type, MinBlockSize, MinimumByteCount, Tag>& dynamic_block_allocator<
        Type, MinBlockSize, MinimumByteCount,
        Tag>::operator=(dynamic_block_allocator<Type, MinBlockSize, MinimumByteCount, Tag>&& other) noexcept
    {
        _finalize(); // clean up the allocations associated with the current
                     // btree

        _freeTree = helios::move(other._freeTree);
        _firstBlock = other._firstBlock;
        _finalBlock = other._finalBlock;

        return *this;
    }

    template <typename Type, size_t MinBlockSize, size_t MinimumByteCount, EMemoryTag Tag>
    inline Type* dynamic_block_allocator<Type, MinBlockSize, MinimumByteCount, Tag>::allocate(size_t count)
    {
        dynamic_block* blk = _alloc_block(count);
        blk = _resize_block(blk, count);
        return blk->get_ptr();
    }

    template <typename Type, size_t MinBlockSize, size_t MinimumByteCount, EMemoryTag Tag>
    inline void dynamic_block_allocator<Type, MinBlockSize, MinimumByteCount, Tag>::release(Type* ptr)
    {
        // get the block from the pointer.  The block is the pointer's address
        // minus the size of the block structure
        u8* p = reinterpret_cast<u8*>(ptr);
        u8* blk = p - sizeof(dynamic_block);
        dynamic_block* block = reinterpret_cast<dynamic_block*>(blk);
        _release_block(block);
    }

    template <typename Type, size_t MinBlockSize, size_t MinimumByteCount, EMemoryTag Tag>
    inline void dynamic_block_allocator<Type, MinBlockSize, MinimumByteCount, Tag>::release_all()
    {
        _finalize();
    }

    template <typename Type, size_t MinBlockSize, size_t MinimumByteCount, EMemoryTag Tag>
    inline void dynamic_block_allocator<Type, MinBlockSize, MinimumByteCount, Tag>::_initialize()
    {
        _firstBlock = nullptr;
        _finalBlock = nullptr;
    }

    template <typename Type, size_t MinBlockSize, size_t MinimumByteCount, EMemoryTag Tag>
    inline void dynamic_block_allocator<Type, MinBlockSize, MinimumByteCount, Tag>::_finalize()
    {
        dynamic_block* block = _firstBlock;
        while (block != nullptr)
        {
            if (block->node == nullptr)
            {
                _release_block(block);
            }

            block = block->next;
        }

        block = _firstBlock;
        while (block != nullptr)
        {
            _firstBlock = block->next;
            mem_free_align_16(block);
            block = _firstBlock;
        }

        _freeTree.clear();
        _firstBlock = nullptr;
        _finalBlock = nullptr;
    }

    template <typename Type, size_t MinBlockSize, size_t MinimumByteCount, EMemoryTag Tag>
    inline void dynamic_block_allocator<Type, MinBlockSize, MinimumByteCount, Tag>::_attach_freed_node(
        typename dynamic_block_allocator<Type, MinBlockSize, MinimumByteCount, Tag>::dynamic_block* blk)
    {
        _freeTree.add(blk->size(), blk);
    }

    template <typename Type, size_t MinBlockSize, size_t MinimumByteCount, EMemoryTag Tag>
    inline void dynamic_block_allocator<Type, MinBlockSize, MinimumByteCount, Tag>::_detach_freed_node(
        typename dynamic_block_allocator<Type, MinBlockSize, MinimumByteCount, Tag>::dynamic_block* blk)
    {
        _freeTree.remove(blk->node);
        blk->node = nullptr;
    }

    template <typename Type, size_t MinBlockSize, size_t MinimumByteCount, EMemoryTag Tag>
    inline typename dynamic_block_allocator<Type, MinBlockSize, MinimumByteCount, Tag>::dynamic_block*
    dynamic_block_allocator<Type, MinBlockSize, MinimumByteCount, Tag>::_alloc_block(size_t num)
    {
        size_t aligned = (num * sizeof(Type) + 15) & ~15; // align to 16 bytes
        // search the free blocks for the smallest free block that is at least
        // the size of the aligned memory size
        dynamic_block* blk = _freeTree.find_smallest_key_greater_than_equals(aligned);
        if (blk != nullptr)
        {
            // found a block in the free tree, remove it from the tree
            _detach_freed_node(blk);
        }
        else
        {
            // no free block found matching the requirements, create one
            size_t requestedMinimum = aligned + sizeof(dynamic_block);
            size_t allocationSize = max(requestedMinimum, MinimumByteCount);
            blk = reinterpret_cast<dynamic_block*>(mem_alloc_align_16(allocationSize, Tag));
            blk->allocator = this;
            blk->size(static_cast<u32>(allocationSize - sizeof(dynamic_block)), true);
            blk->next = nullptr;
            blk->prev = _finalBlock;
            if (_finalBlock != nullptr)
            {
                _finalBlock->next = blk;
            }
            else
            {
                _firstBlock = blk;
            }
            _finalBlock = blk;
            blk->node = nullptr;
        }
        return blk;
    }

    template <typename Type, size_t MinBlockSize, size_t MinimumByteCount, EMemoryTag Tag>
    inline typename dynamic_block_allocator<Type, MinBlockSize, MinimumByteCount, Tag>::dynamic_block*
    dynamic_block_allocator<Type, MinBlockSize, MinimumByteCount, Tag>::_resize_block(
        typename dynamic_block_allocator<Type, MinBlockSize, MinimumByteCount, Tag>::dynamic_block* blk, size_t num)
    {
        size_t aligned = (num * sizeof(Type) + 15) & ~15; // align to 16 bytes
        size_t blkSize = blk->size();

        // requested bytes is larger than the block size, so we need to
        // reallocate memory
        if (aligned > blkSize)
        {
            dynamic_block* next = blk->next;
            // check and see if we can merge blocks
            if (next != nullptr && !next->is_base_block() && next->node != nullptr &&
                blk->size() + sizeof(dynamic_block) + next->size() >= aligned)
            {
                // we can merge, pop the next block out of the free block tree
                _detach_freed_node(next);
                blk->size(blk->size() + sizeof(dynamic_block) + next->size(), blk->is_base_block());
                blk->next = next->next;
                // if the next block has a non-null next block, set that block's
                // previous block to the block blk
                if (next->next != nullptr)
                {
                    next->next->prev = blk;
                }
                else
                {
                    // next was the last block, so set the new last block to blk
                    _finalBlock = blk;
                }
            }
            else
            {
                // couldn't merge the blocks, so let's allocate a new one and
                // free the old
                dynamic_block* old = blk;
                blk = _alloc_block(num);

                // sanity check. modern systems have tons of memory, right?
                if (blk == nullptr)
                {
                    return nullptr;
                }

                // memcpy suffices here, as the movement of constructed objects
                // should be handled by the owner of the allocator.  If it's
                // trivially copyable, we don't have to worry anyways
                memcpy(blk->get_ptr(), old->get_ptr(), old->size());

                _release_block(old);
            }
        }
        // next, check if the block is large enough to be split in two.  If not,
        // we can exit early and return the block.
        if (static_cast<i64>(blk->size() - aligned - sizeof(dynamic_block)) <
            static_cast<i64>(max(MinimumByteCount, sizeof(Type))))
        {
            return blk;
        }

        // split the old block in two pieces
        u8* blkPtr = reinterpret_cast<u8*>(blk);
        u8* newBlkPtr = blkPtr + sizeof(dynamic_block) + aligned;
        dynamic_block* newBlk = reinterpret_cast<dynamic_block*>(newBlkPtr);
        newBlk->size(blk->size() - aligned - sizeof(dynamic_block), false);

        // attach the block like so
        // blk <----> new <----> blk->next
        newBlk->next = blk->next;
        newBlk->prev = blk;
        if (newBlk->next != nullptr)
        {
            newBlk->next->prev = newBlk;
        }
        else
        {

            _finalBlock = newBlk;
        }
        newBlk->node = nullptr;
        blk->next = newBlk;
        blk->size(aligned, blk->is_base_block());

        // add the block to the empty block tree
        _release_block(newBlk);

        return blk;
    }

    template <typename Type, size_t MinBlockSize, size_t MinimumByteCount, EMemoryTag Tag>
    inline void dynamic_block_allocator<Type, MinBlockSize, MinimumByteCount, Tag>::_release_block(
        typename dynamic_block_allocator<Type, MinBlockSize, MinimumByteCount, Tag>::dynamic_block* blk)
    {
        // try to aggregate with the next free block
        dynamic_block* next = blk->next;
        if (next != nullptr && !next->is_base_block() && next->node != nullptr)
        {
            // detach the node to prepare to merge
            _detach_freed_node(next);
            blk->size(blk->size() + sizeof(dynamic_block) + next->size(), blk->is_base_block());
            blk->next = next->next;
            if (next->next != nullptr)
            {
                blk->next->prev = blk;
            }
            else
            {
                _finalBlock = blk;
            }
        }

        // try to aggregate with the previous free block
        dynamic_block* prev = blk->prev;
        if (prev != nullptr && !blk->is_base_block() && prev->node != nullptr)
        {
            _detach_freed_node(prev);
            prev->size(prev->size() + sizeof(dynamic_block) + blk->size(), prev->is_base_block());
            prev->next = blk->next;
            if (blk->next != nullptr)
            {
                blk->next->prev = prev;
            }
            else
            {
                _finalBlock = prev;
            }

            // attach the chain of prev <----> blk <--?--> next to the free tree
            // as a single node
            _attach_freed_node(prev);
        }
        else
        {
            // attach the chain of blk <--?--> next to the free tree as a single
            // node
            _attach_freed_node(blk);
        }
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
        [[nodiscard]] bool operator==(const shared_ptr<Other>& other) const noexcept;

        template <typename Other>
        [[nodiscard]] bool operator!=(const shared_ptr<Other>& other) const noexcept;

        template <typename Output>
        inline friend shared_ptr<Output> dynamic_pointer_cast(const shared_ptr& ptr)
        {
            return ptr.__dynamic_pointer_cast<Output>();
        }

        template <typename Output>
        inline friend shared_ptr<Output> reinterpret_pointer_cast(const shared_ptr& ptr)
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
                res._blk = reinterpret_cast<detail::ControlBlock<Output>*>(_blk);
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
    inline shared_ptr<Type>::shared_ptr(shared_ptr&& ptr) noexcept : _blk(ptr._blk)
    {
        ptr._blk = nullptr;
    }

    template <typename Type>
    template <typename Other>
    inline shared_ptr<Type>::shared_ptr(const shared_ptr<Other>& ptr) : _blk(nullptr)
    {
        static_assert(is_base_of_v<Type, Other> || is_base_of_v<Other, Type> || is_same_v<Other, Type> ||
                          is_same_v<Other, void>,
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
        static_assert(is_base_of_v<Type, Other> || is_base_of_v<Other, Type> || is_same_v<Other, Type> ||
                          is_same_v<Other, void>,
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
        if (reinterpret_cast<void*>(this) != reinterpret_cast<const void*>(&ptr) && _blk != ptr._blk)
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
    inline shared_ptr<Type>& shared_ptr<Type>::operator=(shared_ptr&& ptr) noexcept
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
    inline shared_ptr<Type>& shared_ptr<Type>::operator=(const shared_ptr<Other>& ptr)
    {
        static_assert(is_base_of_v<Type, Other> || is_base_of_v<Other, Type> || is_same_v<Other, Type> ||
                          is_same_v<Other, void>,
                      "Cannot convert between types.");
        if (reinterpret_cast<void*>(this) != reinterpret_cast<const void*>(&ptr) &&
            reinterpret_cast<void*>(_blk) != reinterpret_cast<const void*>(ptr._blk))
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
    inline shared_ptr<Type>& shared_ptr<Type>::operator=(shared_ptr<Other>&& ptr)
    {
        static_assert(is_base_of_v<Type, Other> || is_base_of_v<Other, Type> || is_same_v<Other, Type> ||
                          is_same_v<Other, void>,
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
        static_assert(is_base_of_v<Type, Other> || is_base_of_v<Other, Type> || is_same_v<Other, Type> ||
                          is_same_v<Other, void>,
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
    inline bool shared_ptr<Type>::operator==(const shared_ptr& other) const noexcept
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
    bool shared_ptr<Type>::operator==(const shared_ptr<Other>& other) const noexcept
    {
        return _blk == other._blk;
    }

    template <typename Type>
    template <typename Other>
    bool shared_ptr<Type>::operator!=(const shared_ptr<Other>& other) const noexcept
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
