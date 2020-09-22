#pragma once

#include <helios/containers/utility.hpp>

#include <string>

namespace helios
{
	template<>
	struct hash<std::string>
    {
        size_t operator()(const std::string& str) const
        {
            return std::hash<std::string>{}(str);
        }
	};
}