#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_KHR_vulkan_glsl : enable

layout(location = 0) in vec2 uv;

layout(location = 0) out vec4 outColor;

layout (binding = 0) uniform sampler2D albedoSampler;
layout (binding = 1) uniform sampler2D normalSampler;
layout (binding = 2) uniform sampler2D depthSampler;
layout (binding = 3) uniform LightData {
    vec4 position;
    vec4 direction;
    float intensity;
} lightData;

void main() {
    vec3 albedo = texture(albedoSampler, uv).rgb;
    vec3 normal = texture(normalSampler, uv).xyz;
    float depth = texture(depthSampler, uv).x;

    float ambientTerm = 0.05f;

    float NdotL = clamp(dot(normal, -lightData.direction.xyz), 0.0f, 1.0f);
    float diffuseTerm = NdotL * lightData.intensity;

	outColor = vec4(albedo * (ambientTerm + diffuseTerm), 1.0f);
}

