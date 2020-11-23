#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (binding = 0) uniform TestUBO {
    vec4 test;
} testUBO;

layout(location = 0) in vec3 fragColor;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = vec4(testUBO.test.xyz, 1.0);
}