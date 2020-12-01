#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_KHR_vulkan_glsl : enable

layout(location = 0) in vec2 uv;

layout(location = 0) out vec4 outColor;

layout (binding = 0) uniform sampler2D albedoSampler;
layout (binding = 1) uniform sampler2D normalSampler;

void main() {
    vec3 albedo = texture(albedoSampler, uv).xyz;
    vec3 normal = texture(normalSampler, uv).xyz;
	
	outColor = vec4(albedo, 1.0);
}

