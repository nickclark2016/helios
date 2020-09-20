#pragma once

#include <helios/macros.hpp>
#include <helios/containers/vector.hpp>

#include <string>

namespace helios
{
	class File
    {
    public:
        static vector<uint8_t> read_binary(const std::string& filepath);
        static std::string read_text(const std::string& filepath);
    };
} // namespace helios