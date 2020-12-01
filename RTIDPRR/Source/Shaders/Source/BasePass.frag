#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_KHR_vulkan_glsl : enable

layout(location = 0) in vec3 inPosition;

layout(location = 0) out vec3 outColor;
layout(location = 1) out vec3 outNormal;

void main() {
    outColor = vec3(0.3f, 0.5f, 0.6f);
    outNormal = normalize(cross(dFdx(inPosition), dFdy(inPosition)));
}