#pragma once

#include <helios/containers/vector.hpp>
#include <helios/io/file.hpp>

#include <string>
#include <unordered_map>

namespace helios
{
	class Shader
    {
    public:
        struct ShaderStructMember
        {
            std::string name;
            std::string type;
            u32 offset;
            u32 stride;
            vector<std::pair<u32, bool>> array_sizes;
        };

        struct ShaderStruct
        {
            std::string name;
            vector<ShaderStructMember> members;
        };

        Shader(const std::string& vertexSource,
               const std::string& fragmentSource);
        ~Shader() = default;

    private:
        void _read_reflection(const std::string& source);

        std::unordered_map<std::string, ShaderStruct> _types;
    };
} // namespace helios