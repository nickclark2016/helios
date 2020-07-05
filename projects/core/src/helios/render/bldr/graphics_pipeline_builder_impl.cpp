#include <helios/render/bldr/graphics_pipeline_builder_impl.hpp>

namespace helios
{
    GraphicsPipelineBuilder::VertexInputState getDefaultBindings()
    {
        GraphicsPipelineBuilder::VertexInputState state;

        {
            // Positions
            GraphicsPipelineBuilder::VertexInputAttributeBinding binding = {
                0, 3 * sizeof(f32), EVertexInputRate::VERTEX, 0, EFormat::R32G32B32_SFLOAT, 0};

            state.bindings.push_back(binding);
        }

        {
            // Texture UVs
            GraphicsPipelineBuilder::VertexInputAttributeBinding binding = {
                1, 15 * sizeof(f32), EVertexInputRate::VERTEX, 1, EFormat::R32G32_SFLOAT, 0};

            state.bindings.push_back(binding);
        }

        {
            // Normals
            GraphicsPipelineBuilder::VertexInputAttributeBinding binding = {
                1, 15 * sizeof(f32), EVertexInputRate::VERTEX, 2, EFormat::R32G32B32_SFLOAT, 2 * sizeof(f32)};

            state.bindings.push_back(binding);
        }

        {
            // Tangents
            GraphicsPipelineBuilder::VertexInputAttributeBinding binding = {
                1, 15 * sizeof(f32), EVertexInputRate::VERTEX, 3, EFormat::R32G32B32_SFLOAT, 5 * sizeof(f32)};

            state.bindings.push_back(binding);
        }

        {
            // Bitangents
            GraphicsPipelineBuilder::VertexInputAttributeBinding binding = {
                1, 15 * sizeof(f32), EVertexInputRate::VERTEX, 4, EFormat::R32G32B32_SFLOAT, 8 * sizeof(f32)};

            state.bindings.push_back(binding);
        }

        {
            // Colors
            GraphicsPipelineBuilder::VertexInputAttributeBinding binding = {
                1, 12 * sizeof(f32), EVertexInputRate::VERTEX, 5, EFormat::R32G32B32A32_SFLOAT, 11 * sizeof(f32)};

            state.bindings.push_back(binding);
        }

        return state;
    }
} // namespace helios