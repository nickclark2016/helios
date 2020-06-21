#pragma once

#include <string>
#include <helios/macros.hpp>
#include <helios/math/vector.hpp>
#include <helios/containers/vector.hpp>

namespace helios
{
    struct Vertex
    {
        Vector3f position;
        Vector2f uv;
        Vector3f normal;
        Vector3f tangent;
        Vector3f bitangent;
        Vector4f color;
    };

    struct VertexNoPosition
    {
        Vector2f uv;
        Vector3f normal;
        Vector3f tangent;
        Vector3f bitangent;
        Vector4f color;
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

        vector<u16> triangles;

        vector<Mesh*> subMeshes;

        void build();
        void calculateBitangents();
    };
}