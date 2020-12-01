#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_KHR_vulkan_glsl : enable

layout(location = 0) in vec3 position;

layout(location = 0) out vec3 outPosition;

layout (binding = 0) uniform TestUBO {
    mat4 vp;
} testUBO;


void main() {
    gl_Position = testUBO.vp * vec4(position, 1.0f);
    outPosition = position;
}