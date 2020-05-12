#pragma once

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