#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 position;

layout(location = 0) out vec3 outPosition;
layout(location = 1) out vec3 outColor;

layout (push_constant) uniform CameraMatrices {
    mat4 model;
    mat4 modelViewProjection;
} cameraMatrices;

void main() {
    gl_Position = cameraMatrices.modelViewProjection * vec4(position, 1.0f);
    outPosition = (cameraMatrices.model * vec4(position, 1.0f)).xyz;
    outPosition.y = -outPosition.y;
    outColor = length(position) > 1 ? normalize(abs(position)): abs(position);
}