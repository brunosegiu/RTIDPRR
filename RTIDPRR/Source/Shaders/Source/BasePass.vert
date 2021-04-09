#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_KHR_vulkan_glsl : enable

layout(location = 0) in vec3 position;

layout(location = 0) out vec3 outPosition;
layout(location = 1) out vec3 outColor;
layout(location = 2) out uint outPatchId;

layout (push_constant) uniform CameraMatrices {
    mat4 model;
    mat4 modelViewProjection;
    uint offset;
} pushParams;

void main() {
    gl_Position = pushParams.modelViewProjection * vec4(position, 1.0f);
    
    outPosition = (pushParams.model * vec4(position, 1.0f)).xyz;
    outPosition.y = -outPosition.y;
    
    outColor = length(position) > 1 ? normalize(abs(position)): abs(position);
    
    // Assume a vertex list
    outPatchId = pushParams.offset + (gl_VertexIndex / 3);
}