#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;

layout(location = 0) out vec2 pass_uv;

layout(set = 0, binding = 0) uniform block
{
    mat4 proj;
    mat4 view;
    mat4 modl;
};

void main()
{
    gl_Position = proj * view * modl * vec4(position, 1.0);
    pass_uv = uv;
}