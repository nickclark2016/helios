#pragma once

#include <cstddef>
#include <cstdint>

#define HELIOS_NO_COPY(cls)                                                    \
    cls(const cls&) = delete;                                                  \
    cls& operator=(const cls&) = delete;

#define HELIOS_NO_MOVE(cls)                                                    \
    cls(cls&&) noexcept = delete;                                              \
    cls& operator=(cls&&) noexcept = delete;

#define HELIOS_NO_COPY_MOVE(cls)                                               \
    HELIOS_NO_COPY(cls)                                                        \
    HELIOS_NO_MOVE(cls)

#define HELIOS_NO_DISCARD [[nodiscard]]

#define GEN_HANDLE_DEFN(name, type) using name = type*;
#define PREGEN_HANDLE_DEFN(name, type)                                         \
    class type;                                                                \
    GEN_HANDLE_DEFN(name, type)

using i8 = int8_t;
using u8 = uint8_t;
using i16 = int16_t;
using u16 = uint16_t;
using i32 = int32_t;
using u32 = uint32_t;
using i64 = int64_t;
using u64 = uint64_t;
using f32 = float;
using f64 = double;