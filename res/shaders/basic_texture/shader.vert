#version 450

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 uv;

layout(location = 0) out vec2 pass_uv;

void main() {
    gl_Position = vec4(position, 0.0, 1.0);
    pass_uv = uv;
}