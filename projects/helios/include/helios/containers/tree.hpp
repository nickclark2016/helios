#pragma once

#include <helios/containers/memory_tags.hpp>
#include <helios/macros.hpp>
#include <helios/utility.hpp>

namespace helios
{
    template <typename Type, u32 BlockSize, EMemoryTag Tag>
    class block_allocator;

    template <typename Key, typename Type, u32 MaxChildrenPerNode>
    class btree
    {
    public:
        struct node
        {
            Key key;
            Type* object;
            node* parent;
            node* next;
            node* prev;
            u32 num_children;
            node* first_child;
            node* final_child;

            node(const node&) = default;
            node(node&&) noexcept = default;
            ~node() = default;
            node& operator=(const node&) = default;
            node& operator=(node&&) noexcept = default;

        private:
            node() = default;
        };

        using node_t = node;

        btree();
        btree(const btree&) = delete;
        btree(btree&& other) noexcept;
        ~btree();

        btree& operator=(const btree&) = delete;
        btree& operator=(btree&& other) noexcept;

        node_t* add(const Key& key, Type* value);
        void remove(node_t* node);
        void clear();

        node_t* find_node(const Key& key) const;
        node_t* find_smallest_node_greater_than_equals(const Key& key) const;
        node_t* find_greatest_node_less_than_equals(const Key& key) const;

        Type* find(const Key& key) const;
        Type* find_smallest_key_greater_than_equals(const Key& key);
        Type* find_greatest_key_less_than_equals(const Key& key);

        node_t* get_root() const;
        size_t get_node_count() const;
        node_t* get_next(node_t* node) const;
        node_t* get_next_leaf(node_t* node) const;

    private:
        node_t* _root;
        block_allocator<node_t, 128, EMemoryTag::TAG_BLOCK> _allocator;

        void _initialize();
        void _finalize();

        node_t* _create_node();
        void _release_node(node_t* node);
        void _split_node(node_t* node);
        typename btree<Key, Type, MaxChildrenPerNode>::node_t* _merge_nodes(
            node_t* first, node_t* second);
    };

    template <typename Key, typename Type, u32 MaxChildrenPerNode>
    inline btree<Key, Type, MaxChildrenPerNode>::btree()
    {
        _initialize();
    }

    template <typename Key, typename Type, u32 MaxChildrenPerNode>
    inline btree<Key, Type, MaxChildrenPerNode>::btree(btree&& other) noexcept
    {
        _root = other._root;
        other._root = nullptr;

        _allocator = helios::move(other._allocator);
    }

    template <typename Key, typename Value, u32 MaxChildrenPerNode>
    inline btree<Key, Value, MaxChildrenPerNode>::~btree()
    {
        _finalize();
    }

    template <typename Key, typename Type, u32 MaxChildrenPerNode>
    inline btree<Key, Type, MaxChildrenPerNode>& btree<Key, Type,
                                                       MaxChildrenPerNode>::
    operator=(btree<Key, Type, MaxChildrenPerNode>&& other) noexcept
    {
        _finalize();
        _root = other._root;
        other._root = nullptr;
        _allocator = helios::move(other._allocator);

        return *this;
    }

    template <typename Key, typename Type, u32 MaxChildrenPerNode>
    inline typename btree<Key, Type, MaxChildrenPerNode>::node_t* btree<
        Key, Type, MaxChildrenPerNode>::add(const Key& key, Type* value)
    {
        // check if the root is null.  If so, create the root
        if (_root == nullptr)
        {
            _root = _create_node();
        }

        node_t* newNode;

        // If the root is full, we need to allocate a new node
        // TODO: Investigate if we can do just an equals check and if it's worth
        // it
        if (_root->num_children >= MaxChildrenPerNode)
        {
            newNode = _create_node();
            newNode->key = _root->key;
            newNode->first_child = _root;
            newNode->final_child = _root;
            newNode->num_children = 1;
            _root->parent = newNode;
            // The node is full, split it between nodes
            _split_node(_root);
            _root = newNode;
        }

        newNode = _create_node();
        newNode->key = key;
        newNode->object = value;

        node_t* node = _root;
        node_t* child;
        while (node->first_child != nullptr)
        {
            if (key > node->key)
            {
                node->key = key;
            }

            // determine first child of the node who's key is less than or equal
            // to the key of the child node
            child = node->first_child;
            while (child->next != nullptr)
            {
                if (key <= child->key)
                {
                    break;
                }
                child = child->next;
            }

            // check if the child owns an object
            if (child->object)
            {
                // if less than or equal to, insert before the child, else
                // insert after
                if (key <= child->key)
                {
                    if (child->prev == nullptr)
                    {
                        //   * node
                        //  /
                        // * newNode
                        node->first_child = newNode;
                    }
                    else
                    {
                        // child->prev <----> newNode
                        child->prev->next = newNode;
                    }

                    // child->prev <----> newNode <----> child
                    newNode->prev = child->prev;
                    newNode->next = child;
                    child->prev = newNode;
                }
                else
                {
                    if (child->next == nullptr)
                    {
                        // * node
                        //  \
                        //   * newNode
                        node->final_child = newNode;
                    }
                    else
                    {
                        // newNode <----> child->next
                        child->next->prev = newNode;
                    }

                    // child <----> newNode <----> child->next
                    newNode->prev = child;
                    newNode->next = child->next;
                    child->next = newNode;
                }

                // set newNode's parent pointer and increment the child count
                newNode->parent = node;
                ++(node->num_children);

                return newNode;
            }

            // If the node is full, split it
            if (child->num_children >= MaxChildrenPerNode)
            {
                _split_node(child);
                if (key <= child->prev->key)
                {
                    child = child->prev;
                }
            }

            node = child;
        }

        // if we are here, the root node was empty
        newNode->parent = _root;
        _root->key = key;
        _root->first_child = newNode;
        _root->final_child = newNode;
        ++(_root->num_children);
        return newNode;
    }

    template <typename Key, typename Type, u32 MaxChildrenPerNode>
    inline void btree<Key, Type, MaxChildrenPerNode>::remove(node_t* node)
    {
        // detach node from parent
        if (node->prev != nullptr)
        {
            node->prev->next = node->next;
        }
        else
        {
            node->parent->first_child = node->next;
        }

        if (node->next != nullptr)
        {
            node->next->prev = node->prev;
        }
        else
        {
            node->parent->final_child = node->prev;
        }

        --(node->parent->num_children);

        // merge nodes up the parent chain until a node gets too many nodes, the
        // split it
        node_t* parent = node->parent;
        while (parent != _root && parent->num_children < 2)
        {
            if (parent->next != nullptr)
            {
                // merge with next node
                parent = _merge_nodes(parent, parent->next);
            }
            else
            {
                // merge with previous node
                parent = _merge_nodes(parent->prev, parent);
            }

            // the parent's key must be less than or equal to the its final
            // child's key
            if (parent->key > parent->final_child->key)
            {
                parent->key = parent->final_child->key;
            }

            // parent got merged, check how many children it now has and split
            // if needed
            if (parent->num_children > MaxChildrenPerNode)
            {
                _split_node(parent);
                break;
            }

            parent = parent->parent;
        }

        // go up the chain and verify that the key is not greater than that of
        // the key of its right most child
        while (parent != nullptr && parent->final_child != nullptr)
        {
            if (parent->key <= parent->final_child->key)
            {
                parent = parent->parent;
            }
            else
            {
                parent->key = parent->final_child->key;
            }
        }

        _release_node(node);

        // if the root only has one child, that child is the new root
        if (_root->num_children == 1 && _root->first_child->object == nullptr)
        {
            node_t* prev_root = _root;
            _root->first_child->parent = nullptr;
            _root = _root->first_child;
            _release_node(prev_root);
        }
    }

    template <typename Key, typename Type, u32 MaxChildrenPerNode>
    inline void btree<Key, Type, MaxChildrenPerNode>::clear()
    {
        _finalize();
    }

    template <typename Key, typename Type, u32 MaxChildrenPerNode>
    inline typename btree<Key, Type, MaxChildrenPerNode>::node_t* btree<
        Key, Type, MaxChildrenPerNode>::find_node(const Key& key) const
    {
        node_t* node = _root->first_child;
        // if the node is not null, check to see if it contains the value
        while (node != nullptr)
        {
            // if the key is greater than or equal to, break because we've found
            // the correct node to search the children of
            while (node->next != nullptr)
            {
                if (node->key >= key)
                {
                    break;
                }
                node = node->next;
            }

            // if the node's object contained is non-null and the key is
            // equivalent to the parameter key, return the node
            if (node->object != nullptr)
            {
                return node->key == key ? node : nullptr;
            }
            node = node->first_child;
        }
        return nullptr;
    }

    template <typename Key, typename Type, u32 MaxChildrenPerNode>
    inline typename btree<Key, Type, MaxChildrenPerNode>::node_t* btree<
        Key, Type, MaxChildrenPerNode>::
        find_smallest_node_greater_than_equals(const Key& key) const
    {
        // if the root is non-null, search for the node such that the key of the
        // node is greater than or equal to the parameter key and the key is as
        // far to the left in the tree as is possible while meeting the first
        // criteria
        if (_root != nullptr)
        {
            node_t* node = _root->first_child;
            while (node != nullptr)
            {
                while (node->next != nullptr)
                {
                    if (node->key >= key)
                    {
                        break;
                    }
                    node = node->next;
                }

                // only perform key check if the object is non-null
                if (node->object != nullptr)
                {
                    return node->key >= key ? node : nullptr;
                }
                node = node->first_child;
            }
        }
        return nullptr;
    }

    template <typename Key, typename Type, u32 MaxChildrenPerNode>
    inline typename btree<Key, Type, MaxChildrenPerNode>::node_t* btree<
        Key, Type,
        MaxChildrenPerNode>::find_greatest_node_less_than_equals(const Key& key)
        const
    {
        // if the root is non-null, search for the node such that the key of the
        // node is less than or equal to the parameter key and the key is as
        // far to the right in the tree as is possible while meeting the first
        // criteria
        if (_root != nullptr)
        {
            node_t* node = _root->first_child;
            node_t* smaller_node = nullptr;
            while (node != nullptr)
            {
                while (node->next != nullptr)
                {
                    if (node->key >= key)
                    {
                        break;
                    }
                    smaller_node = node;
                    node = node->first_child;
                }

                // only perform key check if the object is non-null
                if (node->object != nullptr)
                {
                    if (node->key <= key)
                    {
                        return node;
                    }
                    else if (smaller_node == nullptr)
                    {
                        return nullptr;
                    }
                    else
                    {
                        if (smaller_node->object != nullptr)
                        {
                            return smaller_node;
                        }
                        node = smaller_node;
                    }
                }
            }
        }
        return nullptr;
    }

    template <typename Key, typename Type, u32 MaxChildrenPerNode>
    inline Type* btree<Key, Type, MaxChildrenPerNode>::find(
        const Key& key) const
    {
        node_t* res = find_node(key);
        return res ? res->object : nullptr;
    }

    template <typename Key, typename Type, u32 MaxChildrenPerNode>
    inline Type* btree<Key, Type, MaxChildrenPerNode>::
        find_smallest_key_greater_than_equals(const Key& key)
    {
        node_t* node = find_smallest_node_greater_than_equals(key);
        return node ? node->object : nullptr;
    }

    template <typename Key, typename Type, u32 MaxChildrenPerNode>
    inline Type* btree<Key, Type, MaxChildrenPerNode>::
        find_greatest_key_less_than_equals(const Key& key)
    {
        node_t* node = find_greatest_node_less_than_equals(key);
        return node ? node->object : nullptr;
    }

    template <typename Key, typename Type, u32 MaxChildrenPerNode>
    inline typename btree<Key, Type, MaxChildrenPerNode>::node_t* btree<
        Key, Type, MaxChildrenPerNode>::get_root() const
    {
        return _root;
    }

    template <typename Key, typename Type, u32 MaxChildrenPerNode>
    inline size_t btree<Key, Type, MaxChildrenPerNode>::get_node_count() const
    {
        return _allocator.allocation_count();
    }

    template <typename Key, typename Type, u32 MaxChildrenPerNode>
    inline typename btree<Key, Type, MaxChildrenPerNode>::node_t* btree<
        Key, Type, MaxChildrenPerNode>::
        get_next(
            typename btree<Key, Type, MaxChildrenPerNode>::node_t* node) const
    {
        if (node->first_child != nullptr)
        {
            return node->first_child;
        }
        while (node != nullptr && node->next != nullptr)
        {
            node = node->parent;
        }
        return node;
    }

    template <typename Key, typename Type, u32 MaxChildrenPerNode>
    inline typename btree<Key, Type, MaxChildrenPerNode>::node_t* btree<
        Key, Type, MaxChildrenPerNode>::
        get_next_leaf(
            typename btree<Key, Type, MaxChildrenPerNode>::node_t* node) const
    {
        // if there is a first_child, go all the way down to the left most child
        // of the left most leaf of the node
        if (node->first_child != nullptr)
        {
            while (node->first_child != nullptr)
            {
                node = node->first_child;
            }
            return node;
        }

        // go all the way up until the node doesn't have a sibling to the right
        while (node != nullptr && node->next == nullptr)
        {
            node = node->parent;
        }

        // if the node is non-null, check the node for the next leaf
        if (node != nullptr)
        {
            // go to the next node, then go down to the first child
            node = node->next;
            while (node->first_child != nullptr)
            {
                node = node->first_child;
            }

            return node;
        }

        return nullptr;
    }

    template <typename Key, typename Type, u32 MaxChildrenPerNode>
    inline void btree<Key, Type, MaxChildrenPerNode>::_initialize()
    {
        _root = _create_node();
    }

    template <typename Key, typename Type, u32 MaxChildrenPerNode>
    inline void btree<Key, Type, MaxChildrenPerNode>::_finalize()
    {
        // clean up all the memory associated with the allocator
        _allocator.release_all();
        _root = nullptr;
    }

    template <typename Key, typename Type, u32 MaxChildrenPerNode>
    inline typename btree<Key, Type, MaxChildrenPerNode>::node_t* btree<
        Key, Type, MaxChildrenPerNode>::_create_node()
    {
        node_t* n = _allocator.allocate();
        n->key = Key();
        n->object = nullptr;
        n->parent = nullptr;
        n->next = nullptr;
        n->prev = nullptr;
        n->num_children = 0;
        n->first_child = nullptr;
        n->final_child = nullptr;
        return n;
    }

    template <typename Key, typename Type, u32 MaxChildrenPerNode>
    void btree<Key, Type, MaxChildrenPerNode>::_release_node(
        typename btree<Key, Type, MaxChildrenPerNode>::node_t* node)
    {
        node->~node_t();
        _allocator.release(node);
    }

    template <typename Key, typename Type, u32 MaxChildrenPerNode>
    inline void btree<Key, Type, MaxChildrenPerNode>::_split_node(
        typename btree<Key, Type, MaxChildrenPerNode>::node_t* node)
    {
        node_t* newNode = _create_node();
        newNode->parent = node;

        node_t* child = node->first_child;
        child->parent = newNode;

        // handled first 3 children, move to the next
        // split the chilren over the old node and the new node
        for (i32 i = 3; node->num_children; i++)
        {
            child = child->next;
            child->parent = newNode;
        }

        // copy child value to new node
        u32 child_count = child->num_children / 2;
        newNode->key = child->key;
        newNode->num_children = child_count;
        newNode->first_child = child->first_child;
        newNode->final_child = child;

        node->num_children -=
            child_count; // reduce by the amount of children moved
        node->first_child = child->next;

        // if the node has siblings, add it to the implicit sibling chain
        if (node->prev != nullptr)
        {
            node->prev->next = newNode;
        }
        else // if the node has no siblings, add it as the first child of the
             // parent
        {
            node->parent->first_child = newNode;
        }
        // node->prev <----> newNode <----> node
        newNode->prev = node->prev;
        newNode->next = node;
        node->prev = newNode;
        ++(node->parent->num_children);
    }

    template <typename Key, typename Type, u32 MaxChildrenPerNode>
    inline typename btree<Key, Type, MaxChildrenPerNode>::node_t* btree<
        Key, Type, MaxChildrenPerNode>::
        _merge_nodes(
            typename btree<Key, Type, MaxChildrenPerNode>::node_t* left,
            typename btree<Key, Type, MaxChildrenPerNode>::node_t* right)
    {
        node_t* child = left->first_child;
        while (child->next != nullptr)
        {
            child->parent = right;
        }
        child->parent = right;
        child->next = right->first_child;
        right->first_child->prev = child;
        right->first_child = left->first_child;
        right->num_children += left->num_children;

        // detach left node from the parent
        if (left->prev == nullptr)
        {
            left->parent->first_child = right;
        }
        else
        {
            left->prev->next = right;
        }
        right->prev = left->prev;
        --(right->parent->num_children);
        _release_node(left);
        return right;
    }
} // namespace helios