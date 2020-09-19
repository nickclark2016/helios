#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normal;

layout(location = 0) out vec3 pass_position;
layout(location = 1) out vec2 pass_uv;
layout(location = 2) out vec3 pass_normal;

layout(set = 0, binding = 0) uniform sceneBlock
{
    mat4 proj;
    mat4 view;
    vec3 cameraPos;
};

layout(set = 1, binding = 0) uniform modelData
{
    mat4 modl[256];
};

void main()
{
    gl_Position = proj * view * modl[gl_InstanceIndex] * vec4(position, 1.0);
    pass_position = vec3(modl[gl_InstanceIndex] * vec4(position, 1.0));
    pass_uv = uv;
    pass_normal = mat3(modl[gl_InstanceIndex]) * normal;
}