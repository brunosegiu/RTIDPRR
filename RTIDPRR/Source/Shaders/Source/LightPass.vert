#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_KHR_vulkan_glsl : enable

layout (location = 0) out vec2 uv;

vec2 positions[3] = vec2[](
    vec2(-1.0f,  1.0f),
    vec2(-1.0f, -3.0f),
    vec2( 3.0f,  1.0f)
);

void main() {
    float x = -1.0f + float((gl_VertexIndex & 1) << 2);
    float y = -1.0f + float((gl_VertexIndex & 2) << 1);
    uv.x = (x+1.0f)*0.5f;
    uv.y = (y+1.0f)*0.5f;
    gl_Position = vec4(x, y, 0.0f, 1.0f);
}