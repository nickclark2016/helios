#include <helios/containers/chunk_slot_map.hpp>
#include <helios/containers/slot_map.hpp>

namespace helios
{
    slot_key::slot_key(u32 index, u32 generation)
        : _index(index), _generation(generation)
    {
    }

    chunk_slot_key::chunk_slot_key(u32 index, u32 generation)
        : _index(index), _generation(generation)
    {
    }
} // namespace helios
