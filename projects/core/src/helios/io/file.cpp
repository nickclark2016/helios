#include <helios/io/file.hpp>

#include <fstream>

namespace helios
{
    vector<uint8_t> File::read_binary(const std::string& filepath)
    {
        vector<uint8_t> bytes;

        std::ifstream file(filepath, std::ios::ate | std::ios::binary);
        if (file.is_open())
        {
            const size_t sz = static_cast<size_t>(file.tellg());
            bytes.resize(sz);
            file.seekg(0);
            file.read(reinterpret_cast<char*>(bytes.data()), sz);
            file.close();
        }

        return bytes;
    }
    
    std::string File::read_text(const std::string& filepath)
    {
        std::ifstream file(filepath);
        return std::string(std::istreambuf_iterator<char>(file),
                           std::istreambuf_iterator<char>());
    }
} // namespace helios