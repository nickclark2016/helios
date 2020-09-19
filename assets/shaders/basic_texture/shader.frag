#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 pass_uv;

layout(set = 0, binding = 0) uniform block
{
    vec3 color;
};

layout(set = 0, binding = 1) uniform sampler2D texSampler;

layout(location = 0) out vec4 outColor;

void main()
{
    // outColor = vec4(color, 1.0);
    outColor = texture(texSampler, pass_uv);
}
