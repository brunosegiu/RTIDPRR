#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 position;

layout(location = 0) out vec3 outColor;
layout(location = 1) out uint outPatchId;

layout (push_constant) uniform CameraMatrices {
    mat4 model;
    mat4 modelViewProjection;
    uint offset;
} pushParams;

void main() {
    gl_Position = pushParams.modelViewProjection * vec4(position, 1.0f);
    outColor = length(position) > 1 ? normalize(abs(position)): abs(position);
    outPatchId = pushParams.offset + (gl_VertexIndex / 3);
}