#pragma once

#include <helios/containers/vector.hpp>
#include <helios/io/file.hpp>
#include <helios/render/graphics.hpp>

#include <string>
#include <unordered_map>

#include <helios/containers/stl_hashes.hpp>
#include <helios/containers/unordered_map.hpp>

namespace helios
{
    /// <summary>
    /// Represents a struct member inside a shader structured type.
    /// </summary>
    struct ShaderStructMember
    {
        /// <summary>
        /// Name of the member.
        /// </summary>
        std::string name;

        /// <summary>
        /// Type of the member. It is guaranteed to either point to a <see cref="ShaderStruct" /> or a built in type.
        /// </summary>
        std::string type;

        /// <summary>
        /// The number of bytes from the first byte of the <see cref="ShaderStruct" /> structure to the first byte of this member.
        /// </summary>
        u32 offset;

        /// <summary>
        /// The number of bytes between two consecutive elements in the same buffer.
        /// </summary>
        u32 stride;

        /// <summary>
        /// If the member is an array, specifies how many elements are in each dimension of the array.  If the size is paired with a true value, the size is constant.
        /// </summary>
        vector<pair<u32, bool>> array_sizes;
    };

    /// <summary>
    /// Represents a structured type inside a shader.
    /// </summary>
    struct ShaderStruct
    {
        /// <summary>
        /// The name of the structured type.
        /// </summary>
        std::string name;

        /// <summary>
        /// A vector of all members in the structured type.
        /// </summary>
        vector<ShaderStructMember> members;
    };
    
    /// <summary>
    /// Supported texture types in a shader
    /// </summary>
    enum class ETextureType
    {
        /// <summary>
        /// An image that comes from another pass' outputs.
        /// </summary>
        INPUT_TEXTURE,
        
        /// <summary>
        /// A combination image sampler that is used as an image input.
        /// </summary>
        SAMPLED_TEXTURE,

        /// <summary>
        /// A image that is used as an input/output texture for general compute operations.
        /// </summary>
        STORAGE_TEXTURE,
    };

    /// <summary>
    /// Represents a shader texture
    /// </summary>
    struct ShaderTexture
    {
        /// <summary>
        /// The name of the texture.
        /// </summary>
        std::string name;

        /// <summary>
        /// The number of textures at this binding point.
        /// </summary>
        u32 count;

        /// <summary>
        /// The set that the texture is bound to.
        /// </summary>
        u32 set;

        /// <summary>
        /// The binding in a the set that the texture is bound to.
        /// </summary>
        u32 binding;

        /// <summary>
        /// The type of the texture.
        /// </summary>
        ETextureType type;
    };

    /// <summary>
    /// Supported buffer types in a shader
    /// </summary>
    enum class EBufferType
    {
        /// <summary>
        /// A small, read only buffer that is used to store uniform values for the shader.
        /// </summary>
        UNIFORM_BUFFER,

        /// <summary>
        /// A large, read/write buffer that is used as a source to read value in a shader, as well as write values from a shader.
        /// </summary>
        STORAGE_BUFFER
    };

    /// <summary>
    /// Represents a shader buffer
    /// </summary>
    struct ShaderBuffer
    {
        /// <summary>
        /// Name of the buffer in the shader.
        /// </summary>
        std::string name;

        /// <summary>
        /// Name of the type of the buffer. It is guaranteed to either point to a <see cref="ShaderStruct" /> or a built in type.
        /// </summary>
        std::string structType;

        /// <summary>
        /// The number of bytes in the buffer.
        /// </summary>
        u32 size;

        /// <summary>
        /// The set that the buffer is bound to.
        /// </summary>
        u32 set;

        /// <summary>
        /// The binding in a the set that the buffer is bound to.
        /// </summary>
        u32 binding;

        /// <summary>
        /// The number of elements in the buffer.  This is be the same as the <see cref="size"/> divided by the size of the type.
        /// </summary>
        u32 count;

        /// <summary>
        /// The type of the buffer.
        /// </summary>
        EBufferType type;
    };

	/// <summary>
	/// A group of programs to run on the GPU.
	/// </summary>
	class Shader
    {
    public:
        /// <summary>
        /// Creates a new shader.
        /// </summary>
        /// <param name="vertexSource">
        /// The path to the SPIR-V source of the vertex shader, relative to the executable.
        /// </param>
        /// <param name="fragmentSource">
        /// The path to the SPIR-V source of the fragment shader, relative to the executable.
        /// </param>
        Shader(const std::string& vertexSource,
               const std::string& fragmentSource);
        HELIOS_NO_COPY_MOVE(Shader)
        ~Shader();

    private:
        void _read_reflection(const std::string& source);
        void _build_layout();
        IShaderModule* _read_module(const std::string& source);
        void _build_pipeline(const std::string& vertexSource, const std::string fragmentSource);

        unordered_map<std::string, ShaderStruct> _types;
        unordered_map<std::string, ShaderTexture> _textures;
        unordered_map<std::string, ShaderBuffer> _uniformBuffers;
        vector<IDescriptorSetLayout*> _descriptorLayout;
        IPipelineLayout* _layout;
        IGraphicsPipeline* _pipeline;
    };
} // namespace helios