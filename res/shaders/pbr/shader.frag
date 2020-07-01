#version 450
#extension GL_ARB_separate_shader_objects : enable

const float PI = 3.1415926535897932384626433832795;

layout(location = 0) in vec3 pass_position;
layout(location = 1) in vec2 pass_uv;
layout(location = 2) in vec3 pass_normal;

// lighting
struct PointLight
{
    vec3 position;
    vec3 color;
};

// material
struct PBRMaterialParameters
{
    float ao;
};

layout(set = 0, binding = 0) uniform sceneBlock
{
    mat4 proj;
    mat4 view;
    vec3 cameraPos;
};

layout(set = 0, binding = 1) uniform lighting
{
    PointLight point;
};

layout(set = 0, binding = 2) uniform materials
{
    PBRMaterialParameters material;
};

layout(set = 0, binding = 3) uniform sampler2D textures[2];

layout(set = 1, binding = 0) uniform modelData
{
    mat4 modl;
};

layout(location = 0) out vec4 outColor;

// normal distribution function based on Trowbridge-Reitz ggx
float normal_distribution_function(vec3 normal, vec3 halfway, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float nDotH = max(dot(normal, halfway), 0.0);
    float nDotH2 = nDotH * nDotH;

    float nom = a2;
    float denom = (nDotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom /
           max(denom,
               0.001); // prevent divide by zero for roughness=0.0 and NdotH=1.0
}

// geometry roughness function based on the combination of ggx and the
// Schlick-Beckmann approx
float geometry_schlick_ggx_function(float normalDotView, float roughness)
{
    float r = roughness + 1.0;
    float k = r * r / 8.0;

    float num = normalDotView;
    float denom = normalDotView * (1.0 - k) + k;

    return num / denom;
}

// geometry function taking into account microsurface roughness, the normal
// vector, the view vector, and the light vector
float geometry_function(vec3 normal, vec3 view, vec3 light, float roughness)
{
    float normalDotView = max(dot(normal, view), 0.0);   // must be at least 0
    float normalDotLight = max(dot(normal, light), 0.0); // must be at least 0
    float ggx1 = geometry_schlick_ggx_function(normalDotView, roughness);
    float ggx2 = geometry_schlick_ggx_function(normalDotLight, roughness);
    return ggx1 * ggx2;
}

// Fresnel Schlick approximation
vec3 fresnel(float cosTheta, vec3 normalIncidence)
{
    return normalIncidence + (1.0 - normalIncidence) * pow(1.0 - cosTheta, 5.0);
}

void main()
{
    vec3 norm = normalize(pass_normal);
    vec3 viewVec = normalize(cameraPos - pass_position);
    vec3 normalIncidence = vec3(0.04);

    vec4 albedo = texture(textures[0], pass_uv);
    vec4 metallicRough = texture(textures[1], pass_uv);

    normalIncidence = mix(normalIncidence, albedo.rgb, metallicRough.r);

    vec3 lo = vec3(0.0);

    vec3 light = normalize(point.position - pass_position);
    vec3 halfV = normalize(viewVec + light);
    float distance = length(point.position - pass_position);
    float attenuation = 1.0 / distance * distance;
    vec3 radiance = point.color * attenuation;

    float ndf = normal_distribution_function(norm, halfV, metallicRough.r);
    float geom = geometry_function(norm, viewVec, light, metallicRough.r);
    vec3 fres = fresnel(clamp(dot(halfV, viewVec), 0.0, 1.0), normalIncidence);
    vec3 ks = fres;
    vec3 kd = vec3(1.0) - ks;
    kd *= 1.0 - metallicRough.r;

    vec3 num = ndf * geom * fres;
    float denom =
        4.0 * max(dot(norm, viewVec), 0.0) * max(dot(norm, light), 0.0);
    vec3 spec = num / max(denom, 0.001);
    float normDotLight = max(dot(norm, light), 0.0);
    lo += (kd * albedo.rgb / PI + spec) * radiance * normDotLight;

    vec3 ambient = vec3(0.03) * albedo.rgb * material.ao;
    vec3 color = ambient + lo;
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0 / 2.2));

    outColor = vec4(color, 1.0);
}
