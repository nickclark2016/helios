#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 pass_position;
layout(location = 1) in vec2 pass_uv;
layout(location = 2) in vec3 pass_normal;

struct DirectionalLight
{
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

layout(set = 0, binding = 0) uniform block
{
    mat4 proj;
    mat4 view;
    mat4 modl;
    vec3 cameraPos;
};

layout(set = 0, binding = 1) uniform sampler2D texSampler;
layout(set = 0, binding = 2) uniform Lights
{
    DirectionalLight directional;
};

layout(location = 0) out vec4 outColor;

void main()
{
    vec4 albedo = texture(texSampler, pass_uv);

    // compute ambient
    vec3 ambient = directional.ambient * albedo.rgb;

    // compute diffuse
    vec3 norm = normalize(pass_normal);
    vec3 lightDir = normalize(-directional.direction);
    float diff = max(dot(norm, lightDir), 0.1);
    vec3 diffuse = albedo.rgb * diff * directional.diffuse;

    // compute specular
    vec3 viewDir = normalize(cameraPos - pass_position);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 1.0);
    vec3 specular = directional.specular * spec * albedo.rgb;

    outColor = vec4(ambient + diffuse + specular, 1.0);
}
