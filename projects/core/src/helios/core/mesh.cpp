#include <helios/core/mesh.hpp>

#include <fx/gltf.h>

namespace helios
{
    namespace detail
    {
        struct BufferInfo
        {
            fx::gltf::Accessor const* accessor;

            u8 const* data;
            u32 dataStride;
            u32 totalSize;

            bool hasData() const noexcept
            {
                return data != nullptr;
            }
        };

        static u32 calculateDataTypeSize(const fx::gltf::Accessor& accessor) noexcept
        {
            u32 elementSize;
            switch (accessor.componentType)
            {
            case fx::gltf::Accessor::ComponentType::Byte:
            case fx::gltf::Accessor::ComponentType::UnsignedByte:
                elementSize = 1;
                break;

            case fx::gltf::Accessor::ComponentType::Short:
            case fx::gltf::Accessor::ComponentType::UnsignedShort:
                elementSize = 2;
                break;

            case fx::gltf::Accessor::ComponentType::Float:
            case fx::gltf::Accessor::ComponentType::UnsignedInt:
                elementSize = 4;
                break;

            default:
                elementSize = 0;
                break;
            }

            switch (accessor.type)
            {
            case fx::gltf::Accessor::Type::Mat2:
                return 4 * elementSize;
            case fx::gltf::Accessor::Type::Mat3:
                return 9 * elementSize;
            case fx::gltf::Accessor::Type::Mat4:
                return 16 * elementSize;
            case fx::gltf::Accessor::Type::Scalar:
                return elementSize;
            case fx::gltf::Accessor::Type::Vec2:
                return 2 * elementSize;
            case fx::gltf::Accessor::Type::Vec3:
                return 3 * elementSize;
            case fx::gltf::Accessor::Type::Vec4:
                return 4 * elementSize;
            default:
                return 0;
            }
        }

        /*static u32 getAccessorTypeSize(fx::gltf::Accessor const& accessor)
        {
            switch (accessor.componentType)
            {
            case fx::gltf::Accessor::ComponentType::Byte:
            case fx::gltf::Accessor::ComponentType::UnsignedByte:
                return 1;

            case fx::gltf::Accessor::ComponentType::Short:
            case fx::gltf::Accessor::ComponentType::UnsignedShort:
                return 2;

            case fx::gltf::Accessor::ComponentType::Float:
            case fx::gltf::Accessor::ComponentType::UnsignedInt:
                return 4;

            default:
                return 0;
            }
        }*/

        static BufferInfo getData(fx::gltf::Document const& document, fx::gltf::Accessor const& accessor)
        {
            fx::gltf::BufferView const& bufferView = document.bufferViews[accessor.bufferView];
            fx::gltf::Buffer const& buffer = document.buffers[bufferView.buffer];

            const uint32_t dataTypeSize = calculateDataTypeSize(accessor);
            return BufferInfo{&accessor,
                              buffer.data.data() + (static_cast<uint64_t>(bufferView.byteOffset) + accessor.byteOffset),
                              dataTypeSize, accessor.count * dataTypeSize};
        }
    } // namespace detail

    Mesh::Mesh()
    {
    }

    Mesh::Mesh(const std::string& filePath)
    {
        std::string suffix = "gltf";

        const bool isText = (0 == filePath.compare(filePath.length() - suffix.length(), suffix.length(), suffix));
        suffix = "glb";

        const bool isBinary = (0 == filePath.compare(filePath.length() - suffix.length(), suffix.length(), suffix));

        fx::gltf::Document document;

        if (isText)
        {
            document = fx::gltf::LoadFromText(filePath);
        }

        if (isBinary)
        {
            document = fx::gltf::LoadFromBinary(filePath);
        }

        for (const auto& mesh : document.meshes)
        {
            vector<Vector3f> mpositions;
            vector<Vector2f> muvs;
            vector<Vector3f> mnormals;
            vector<Vector3f> mtangents;
            vector<Vector3f> mbitangents;

            vector<u32> mindices;

            for (const auto& primitive : mesh.primitives)
            {
                for (const auto& attribute : primitive.attributes)
                {
                    if (attribute.first == "POSITION")
                    {
                        auto accessor = document.accessors[attribute.second];
                        auto positionBuffer = detail::getData(document, accessor);

                        if (positionBuffer.hasData())
                        {
                            for (uint32_t i = 0; i < positionBuffer.totalSize / sizeof(float); i += 3)
                            {
                                float x = ((float*)positionBuffer.data)[i];
                                float y = ((float*)positionBuffer.data)[i + 1];
                                float z = ((float*)positionBuffer.data)[i + 2];

                                mpositions.push_back({x, y, z});
                            }
                        }
                    }
                    else if (attribute.first == "TEXCOORD_0")
                    {
                        auto accessor = document.accessors[attribute.second];
                        auto uvBuffer = detail::getData(document, accessor);

                        if (uvBuffer.hasData())
                        {
                            for (uint32_t i = 0; i < uvBuffer.totalSize / sizeof(f32); i += 2)
                            {
                                float x = ((float*)uvBuffer.data)[i];
                                float y = ((float*)uvBuffer.data)[i + 1];

                                muvs.push_back({x, y});
                            }
                        }
                    }
                    else if (attribute.first == "NORMAL")
                    {
                        auto accessor = document.accessors[attribute.second];
                        auto normalBuffer = detail::getData(document, accessor);

                        if (normalBuffer.hasData())
                        {
                            for (uint32_t i = 0; i < normalBuffer.totalSize / sizeof(float); i += 3)
                            {
                                float x = ((float*)normalBuffer.data)[i];
                                float y = ((float*)normalBuffer.data)[i + 1];
                                float z = ((float*)normalBuffer.data)[i + 2];

                                mnormals.push_back({x, y, z});
                            }
                        }
                    }
                    else if (attribute.first == "TANGENT")
                    {
                        auto accessor = document.accessors[attribute.second];
                        auto tangentBuffer = detail::getData(document, accessor);

                        if (tangentBuffer.hasData())
                        {
                            for (uint32_t i = 0; i < tangentBuffer.totalSize / sizeof(float); i += 4)
                            {
                                float x = ((float*)tangentBuffer.data)[i];
                                float y = ((float*)tangentBuffer.data)[i + 1];
                                float z = ((float*)tangentBuffer.data)[i + 2];
                                float w = ((float*)tangentBuffer.data)[i + 3];

                                mtangents.push_back({x, y, z});
                                mbitangents.push_back({w, 0, 0});
                            }
                        }
                    }
                }

                auto indexAccessor = document.accessors[primitive.indices];
                auto indexBuffer = detail::getData(document, indexAccessor);

                if (indexBuffer.hasData())
                {
                    for (uint32_t i = 0; i < indexBuffer.totalSize / sizeof(uint16_t); i++)
                    {
                        uint16_t idx = ((uint16_t*)indexBuffer.data)[i];

                        mindices.push_back({idx});
                    }
                }

                Mesh* heliosMesh = new Mesh();
                heliosMesh->positions = mpositions;
                heliosMesh->uvs = muvs;
                heliosMesh->normals = mnormals;
                heliosMesh->tangents = mtangents;
                heliosMesh->bitangents = mbitangents;

                heliosMesh->triangles = mindices;

                heliosMesh->calculateBitangents();
                heliosMesh->build();

                subMeshes.push_back(heliosMesh);
            }
        }
    }

    Mesh::~Mesh()
    {
        for (auto& mesh : subMeshes)
        {
            delete mesh;
        }

        for (auto& buf : _buffers)
        {
            free(buf.data);
        }
        _buffers.clear();
    }

    void Mesh::build()
    {
        vector<Vector3fView> positionView;
        vector<VertexNoPosition> vNoPosition;
        for (size_t i = 0; i < positions.size(); i++)
        {
            positionView.push_back(positions[i]);
            VertexNoPosition v = {};
            if (uvs.size() > i)
            {
                v.uv = uvs[i];
            }
            else
            {
                v.uv = {0, 0};
            }

            if (normals.size() > i)
            {
                v.normal = normals[i];
            }
            else
            {
                v.normal = {0, 0, 0};
            }

            if (tangents.size() > i)
            {
                v.tangent = tangents[i];
            }
            else
            {
                v.tangent = {0, 0, 0};
            }

            if (bitangents.size() > i)
            {
                v.bitangent = bitangents[i];
            }
            else
            {
                v.bitangent = {0, 0, 0};
            }

            if (colors.size() > i)
            {
                v.color = colors[i];
            }
            else
            {
                v.color = {1, 1, 1, 1};
            }

            vNoPosition.push_back(v);
        }

        {
            void* data = malloc(positions.size() * sizeof(Vector3fView));
            memcpy(data, positionView.data(), positionView.size() * sizeof(Vector3fView));
            _buffers.push_back({data, positionView.size() * sizeof(Vector3fView)});
        }

        {
            void* data = malloc(vNoPosition.size() * sizeof(VertexNoPosition));
            memcpy(data, vNoPosition.data(), vNoPosition.size() * sizeof(VertexNoPosition));
            _buffers.push_back({data, vNoPosition.size() * sizeof(VertexNoPosition)});
        }
    }

    void Mesh::calculateBitangents()
    {
        for (size_t i = 0; i < tangents.size(); i++)
        {
            const float w = bitangents[i].x;
            bitangents[i] = normals[i].cross(tangents[i]) * w;
        }
    }

    void* Mesh::readBuffer(const u32 id) const
    {
        return _buffers[id].data;
    }

    u32 Mesh::bufferCount() const
    {
        return static_cast<u32>(_buffers.size());
    }

    u64 Mesh::bufferSize(const u32 id) const
    {
        return _buffers[id].size;
    }
} // namespace helios