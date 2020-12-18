#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 position;

layout (push_constant) uniform CameraMatrices {
    mat4 model;
    mat4 modelViewProjection;
} cameraMatrices;

void main() {
    gl_Position = cameraMatrices.modelViewProjection * vec4(position, 1.0f);
}