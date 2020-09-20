#include <helios/render/shader.hpp>

#include <nlohmann/json.hpp>

#include <utility>

namespace helios
{
    static constexpr const char* ENTRYPOINTS = "entryPoints";
    static constexpr const char* ENTRYPOINT_NAME = "name";
    static constexpr const char* STRUCT_TYPES = "types";
    static constexpr const char* STRUCT_NAME = "name";
    static constexpr const char* STRUCT_MEMBERS = "members";
    static constexpr const char* STRUCT_MEMBER_NAME = "name";
    static constexpr const char* STRUCT_MEMBER_TYPE = "type";
    static constexpr const char* STRUCT_MEMBER_OFFSET = "offset";
    static constexpr const char* STRUCT_MEMBER_STRIDE = "stride";
    static constexpr const char* STRUCT_MEMBER_ARRAY = "array";
    static constexpr const char* STRUCT_MEMBER_ARRAY_LITERAL =
        "array_size_is_literal";

    static constexpr const char* DEFAULT_ENTRYPOINT = "main";

    Shader::Shader(const std::string& vertexSource, const std::string& fragmentSource)
    {
        const vector<uint8_t> vertexSourceBytes = File::read_binary(vertexSource);
        const vector<uint8_t> fragSourceBytes = File::read_binary(fragmentSource);

        _read_reflection(vertexSource + ".refl");
        _read_reflection(fragmentSource + ".refl");
    }

    void Shader::_read_reflection(const std::string& source)
    {
        const std::string refl = File::read_text(source);
        nlohmann::json json = nlohmann::json::parse(refl);

        std::string entrypoint;

        const auto& entrypoints = json[ENTRYPOINTS];
        for (const auto& ep : entrypoints)
        {
            const std::string name = ep[ENTRYPOINT_NAME];
            if (name == DEFAULT_ENTRYPOINT)
            {
                entrypoint = name;
                break;
            }
        }

        const auto& structTypes = json[STRUCT_TYPES];
        for (const auto& [key, type] : structTypes.items())
        {
            ShaderStruct s;
            s.name = type[STRUCT_NAME];

            // skip builtin glsl structs
            if (s.name.find_first_of("gl_") == 0)
            {
                continue;
            }

            for (const auto& member : type[STRUCT_MEMBERS])
            {
                ShaderStructMember m;
                std::string type = member[STRUCT_MEMBER_TYPE];
                if (type[0] == '_')
                {
                    type = structTypes[type][STRUCT_NAME];
                }

                m.name = member[STRUCT_MEMBER_NAME];
                m.type = type;
                if (member.contains(STRUCT_MEMBER_OFFSET))
                {
                    m.offset = member[STRUCT_MEMBER_OFFSET];
                }
                else
                {
                    m.offset = 0;
                }
                if (member.contains(STRUCT_MEMBER_STRIDE))
                {
                    m.stride = member[STRUCT_MEMBER_STRIDE];
                }
                else
                {
                    m.stride = 16;
                }

                if (member.contains(STRUCT_MEMBER_ARRAY))
                {
                    for (const u32 size : member[STRUCT_MEMBER_ARRAY])
                    {
                        m.array_sizes.push_back({size, false});
                    }
                    u32 count = 0;
                    for (const bool literal : member[STRUCT_MEMBER_ARRAY_LITERAL])
                    {
                        m.array_sizes[count++].second = literal;
                    }
                }

                s.members.push_back(m);
            }
            _types[s.name] = s;
        }
    }
} // namespace helios