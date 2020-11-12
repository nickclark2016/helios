#include <helios/render/shader.hpp>

#include <helios/core/engine_context.hpp>
#include <helios/io/file.hpp>
#include <helios/render/graphics.hpp>

#include <nlohmann/json.hpp>

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

    static constexpr const char* TEXTURES = "textures";
    static constexpr const char* TEXTURE_TYPE = "type";
    static constexpr const char* TEXTURE_NAME = "name";
    static constexpr const char* TEXTURE_ARRAY = "array";
    static constexpr const char* TEXTURE_ARRAY_LITERAL = "array_size_is_literal";
    static constexpr const char* TEXTURE_SET = "set";
    static constexpr const char* TEXTURE_BINDING = "binding";

    static constexpr const char* UNIFORM_BUFFERS = "ubos";
    static constexpr const char* UNIFORM_BUFFER_NAME = "name";
    static constexpr const char* UNIFORM_BUFFER_TYPE = "type";
    static constexpr const char* UNIFORM_BUFFER_SET = "set";
    static constexpr const char* UNIFORM_BUFFER_BINDING = "binding";
    static constexpr const char* UNIFORM_BUFFER_BLOCK_SIZE = "block_size";
    static constexpr const char* UNIFORM_ARRAY = "array";

    static constexpr const char* DEFAULT_ENTRYPOINT = "main";

    Shader::Shader(const std::string& vertexSource, const std::string& fragmentSource, IRenderPass* pass,
                   const u32 subpass)
    {
        const vector<uint8_t> vertexSourceBytes = File::read_binary(vertexSource);
        const vector<uint8_t> fragSourceBytes = File::read_binary(fragmentSource);

        _read_reflection(vertexSource + ".refl");
        _read_reflection(fragmentSource + ".refl");
        _build_layout();
        _build_pipeline(vertexSource, fragmentSource);
    }

    Shader::~Shader()
    {
        delete _layout;
        for (auto set : _descriptorLayout)
        {
            delete set;
        }
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

        const auto& textures = json[TEXTURES];
        for (const auto& texture : textures)
        {
            ShaderTexture t;
            t.name = texture[TEXTURE_NAME];
            t.set = texture.contains(TEXTURE_SET) ? (u32)texture[TEXTURE_SET] : 0;
            t.binding = texture.contains(TEXTURE_BINDING) ? (u32)texture[TEXTURE_BINDING] : 0;
            
            if (texture.contains(TEXTURE_ARRAY))
            {
                for (const u32 size : texture[TEXTURE_ARRAY])
                {
                    t.count = size;
                    break;
                }

            }
            else
            {
                t.count = 1;
            }

            _textures[t.name] = t;
        }

        const auto& uniforms = json[UNIFORM_BUFFERS];
        for (const auto& uniform : uniforms)
        {
            ShaderBuffer b;
            b.name = uniform[UNIFORM_BUFFER_NAME];
            std::string type = uniform[UNIFORM_BUFFER_TYPE];
            b.structType = structTypes[type][STRUCT_NAME];
            b.set = uniform[UNIFORM_BUFFER_SET];
            b.binding = uniform[UNIFORM_BUFFER_BINDING];
            b.size = uniform[UNIFORM_BUFFER_BLOCK_SIZE];
            b.count = uniform.contains(UNIFORM_ARRAY) ? (u32)uniform[UNIFORM_ARRAY] : 1;
            b.type = EBufferType::UNIFORM_BUFFER;

            _uniformBuffers[b.name] = b;
        }
    }
    
    void Shader::_build_layout()
    {
        vector<vector<DescriptorSetLayoutBinding>> bindings;

        for (const auto& [name, uniformBuffer] : _uniformBuffers)
        {
            if (uniformBuffer.set >= bindings.size())
            {
                bindings.resize(uniformBuffer.set + 1);
            }

            auto& setBindings = bindings[uniformBuffer.set];

            if (uniformBuffer.binding >= setBindings.size())
            {
                setBindings.resize(uniformBuffer.binding + 1);
            }

            DescriptorSetLayoutBinding binding;
            binding.binding = uniformBuffer.binding;
            binding.count = 1;
            // TODO: Determine which stages need to use binding
            binding.stages = SHADER_STAGE_VERTEX_BIT | SHADER_STAGE_FRAGMENT_BIT;
            binding.type = EDescriptorType::UNIFORM_BUFFER_DYNAMIC;

            setBindings[binding.binding] = binding;
        }

        for (const auto& [name, texture] : _textures)
        {
            if (texture.set >= bindings.size())
            {
                bindings.resize(texture.set + 1);
            }

            auto& setBindings = bindings[texture.set];

            if (texture.binding >= setBindings.size())
            {
                setBindings.resize(texture.binding + 1);
            }

            DescriptorSetLayoutBinding binding;
            binding.binding = texture.binding;
            binding.count = texture.count;
            // TODO: Determine which stages need to use binding
            binding.stages = SHADER_STAGE_FRAGMENT_BIT;
            binding.type = texture.type == ETextureType::SAMPLED_TEXTURE ? EDescriptorType::COMBINED_IMAGE_SAMPLER : EDescriptorType::STORAGE_IMAGE;

            setBindings[binding.binding] = binding;
        }

        for (auto& bindingSet : bindings)
        {
            auto setLayout = DescriptorSetLayoutBuilder()
                .device(&EngineContext::instance().render().device())
                .bindings(bindingSet)
                .build();
            _descriptorLayout.push_back(setLayout);
        }

        auto layout = PipelineLayoutBuilder()
                          .device(&EngineContext::instance().render().device())
                          .layouts(_descriptorLayout)
                          .build();

        _layout = layout;
    }

    IShaderModule* Shader::_read_module(const std::string& source)
    {
        vector<u8> src = File::read_binary(source);
        IDevice& device = EngineContext::instance().render().device();

        return ShaderModuleBuilder().device(&device).source(src).build();
    }

    void Shader::_build_pipeline(const std::string& vertexSource, const std::string fragmentSource)
    {
        IShaderModule* vertexModule = _read_module(vertexSource);
        IShaderModule* fragmentModule = _read_module(fragmentSource);

        auto& window = EngineContext::instance().window();
        const u32 width = window.width();
        const u32 height = window.height();

        GraphicsPipelineBuilder::InputAssemblyState inputAssemblyState;
        inputAssemblyState.primitiveRestartEnable = false;
        inputAssemblyState.topology = EPrimitiveTopology::TRIANGLE_LIST;

        GraphicsPipelineBuilder::TessellationState tessellationState;
        tessellationState.patchControlPoints = 1;

        GraphicsPipelineBuilder::ViewportState viewportState;
        viewportState.viewports.push_back({0.0f, 0.0f, static_cast<const f32>(width), static_cast<const f32>(height), 0.0f, 1.0f});
        viewportState.scissorRegions.push_back({0, 0, width, height});

        GraphicsPipelineBuilder::RasterizationState rasterState;
        rasterState.depthClampEnable = false;
        rasterState.rasterizerDiscardEnable = true;
        rasterState.mode = EPolygonMode::FILL;
        rasterState.cullMode = CULL_MODE_BACK;
        rasterState.frontFace = EVertexWindingOrder::COUNTER_CLOCKWISE;
        rasterState.depthBiasEnable = false;

        GraphicsPipelineBuilder::MultisampleState multisampleState;
        multisampleState.samples = SAMPLE_COUNT_1;
        multisampleState.sampleShadingEnable = false;
        multisampleState.alphaToCoverage = false;
        multisampleState.alphaToOne = false;

        GraphicsPipelineBuilder::DepthStencilState depthState;
        depthState.depthTestEnable = true;
        depthState.depthWriteEnable = true;
        depthState.depthCompareOp = ECompareOp::LESS;
        depthState.depthBoundsTestEnable = true;
        depthState.stencilTestEnable = false;
        depthState.minDepthBounds = 0.0f;
        depthState.maxDepthBounds = 1.0f;

        GraphicsPipelineBuilder::ColorBlendAttachmentState colorBlendAttachment;

//        auto pipeline = GraphicsPipelineBuilder()
//                            .vertex(vertexModule)
//                            .fragment(fragmentModule)
//                            .assembly(inputAssemblyState)
//                            .tessellation(tessellationState)
//                            .viewports(viewportState)
//                            .rasterization(rasterState)
//                            .multisample(multisampleState)
//                            .depthStencil(depthState)
//                            .layout(_layout)
//                            .build();
//        _pipeline = pipeline;

        delete vertexModule;
        delete fragmentModule;
    }
} // namespace helios