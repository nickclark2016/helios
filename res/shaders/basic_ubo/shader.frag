#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform block {
    vec3 color;
};

layout(location = 0) out vec4 outColor;

void main() {
    outColor = vec4(color, 1.0);
}
