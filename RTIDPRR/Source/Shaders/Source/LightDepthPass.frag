#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec3 outColor;
layout(location = 1) out uint outPatchId;

layout(location = 0) in vec3 inColor;
layout(location = 1) flat in uint inPatchId;

void main() {
	outColor = inColor;
	outPatchId = inPatchId;
}