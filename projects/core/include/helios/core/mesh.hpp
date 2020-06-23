#pragma once

#include <helios/containers/vector.hpp>
#include <helios/macros.hpp>
#include <helios/math/vector.hpp>
#include <string>

namespace helios
{
    struct Vertex
    {
        Vector3fView position;
        Vector2fView uv;
        Vector3fView normal;
        Vector3fView tangent;
        Vector3fView bitangent;
        Vector4fView color;
    };

    struct VertexNoPosition
    {
        Vector2fView uv;
        Vector3fView normal;
        Vector3fView tangent;
        Vector3fView bitangent;
        Vector4fView color;
    };

    class Mesh
    {
    public:
        Mesh();
        Mesh(const std::string& filePath);
        ~Mesh();
        HELIOS_NO_COPY_MOVE(Mesh);

        vector<Vector3f> positions;
        vector<Vector2f> uvs;
        vector<Vector3f> normals;
        vector<Vector3f> tangents;
        vector<Vector3f> bitangents;
        vector<Vector4f> colors;

        vector<u32> triangles;

        vector<Mesh*> subMeshes;

        void build();
        void calculateBitangents();

        void* readBuffer(const u32 id) const;
        u32 bufferCount() const;
        u64 bufferSize(const u32 id) const;

    private:
        struct InternalBuffer
        {
            void* data;
            u64 size;
        };
        vector<InternalBuffer> _buffers;
    };
} // namespace helios