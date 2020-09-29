#pragma once

#include <iostream>
#include <string>

namespace helios
{

    // Procedure: stringify
    template <typename T>
    void ostreamize(std::ostringstream& oss, T&& token)
    {
        oss << std::forward<T>(token);
    }

    // Procedure: stringify
    template <typename T, typename... Rest>
    void ostreamize(std::ostringstream& oss, T&& token, Rest&&... rest)
    {
        oss << std::forward<T>(token);
        ostreamize(oss, std::forward<Rest>(rest)...);
    }

} // namespace helios
