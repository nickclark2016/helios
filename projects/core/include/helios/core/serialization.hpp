#pragma once

#include <helios/math/vector.hpp>

namespace helios
{
    template <typename T>
    u64 alignment()
    {
        return sizeof(T);
    }

    template <typename T>
    u64 align(u64 start)
    {
        u64 multiple = alignment<T>();
        if (multiple == 0)
        {
            return start;
        }

        u64 mod = start % multiple;
        if (mod == 0)
        {
            return start;
        }

        return start + multiple - mod;
    }

    template <typename T>
    u64 write(u8* output, u64 start, T& value)
    {
        const u64 offset = align<T>(start);
        memcpy(output + offset, &value, sizeof(T));
        return offset + sizeof(T);
    }

    template <typename T>
    u64 read(u8* input, u64 start, T& value)
    {
        const u64 offset = align<T>(start);
        memcpy(&value, input + offset, sizeof(T));
        return offset + sizeof(T);
    }

    template <>
    u64 write<Vector2f>(u8* output, u64 start, Vector2f& value)
    {
        u64 offset = align<Vector2f>(start);
        offset = write(output, offset, value.x);
        offset = write(output, offset, value.y);
        return offset;
    }

    template <>
    u64 read<Vector2f>(u8* input, u64 start, Vector2f& value)
    {
        u64 offset = align<Vector2f>(start);
        offset = read(input, offset, value.x);
        offset = read(input, offset, value.y);
        return offset;
    }

    template <>
    u64 alignment<Vector2f>()
    {
        return 2 * sizeof(f32);
    }

    template <>
    u64 write<Vector3f>(u8* output, u64 start, Vector3f& value)
    {
        u64 offset = align<Vector2f>(start);
        offset = write(output, offset, value.x);
        offset = write(output, offset, value.y);
        offset = write(output, offset, value.z);
        return offset;
    }

    template <>
    u64 read<Vector3f>(u8* input, u64 start, Vector3f& value)
    {
        u64 offset = align<Vector2f>(start);
        offset = read(input, offset, value.x);
        offset = read(input, offset, value.y);
        offset = read(input, offset, value.z);
        return offset;
    }

    template <>
    u64 alignment<Vector3f>()
    {
        return 4 * sizeof(f32);
    }

    template <>
    u64 write<Vector4f>(u8* output, u64 start, Vector4f& value)
    {
        u64 offset = align<Vector2f>(start);
        offset = write(output, offset, value.x);
        offset = write(output, offset, value.y);
        offset = write(output, offset, value.z);
        offset = write(output, offset, value.w);
        return offset;
    }

    template <>
    u64 read<Vector4f>(u8* input, u64 start, Vector4f& value)
    {
        u64 offset = align<Vector2f>(start);
        offset = read(input, offset, value.x);
        offset = read(input, offset, value.y);
        offset = read(input, offset, value.z);
        offset = read(input, offset, value.w);
        return offset;
    }

    template <>
    u64 alignment<Vector4f>()
    {
        return 4 * sizeof(f32);
    }
}