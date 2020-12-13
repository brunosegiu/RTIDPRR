#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) out vec2 uv;

vec2 positions[3] = vec2[](
    vec2(-1.0f, -3.0f),
    vec2(-1.0f,  1.0f),
    vec2( 3.0f,  1.0f)
);

void main() {
    gl_Position = vec4(positions[gl_VertexIndex],0.0f,1.0f);
    uv = 0.5f * gl_Position.xy + vec2(0.5f);
}