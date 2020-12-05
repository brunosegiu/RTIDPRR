#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_KHR_vulkan_glsl : enable

layout(location = 0) in vec2 uv;

layout(location = 0) out vec4 outColor;

layout (binding = 0) uniform sampler2D albedoSampler;
layout (binding = 1) uniform sampler2D normalSampler;
layout (binding = 2) uniform sampler2D depthSampler;

void main() {
    vec3 albedo = texture(albedoSampler, uv).rgb;
    vec3 normal = texture(normalSampler, uv).xyz;
    float depth = texture(depthSampler, uv).x;

	outColor = vec4(albedo * (0.05f + vec3(clamp(dot(normal, normalize(vec3(-1,-1,0))), 0, 1))), 1.0);
}

