#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) flat in uint inPatchId;

layout(location = 0) out vec3 outColor;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec3 outPosition;
layout(location = 3) out uint outPatchId;

void main() {
    outColor = inColor;
    outNormal = normalize(cross(dFdx(inPosition), dFdy(inPosition)));
    outPosition = inPosition;
    outPatchId = inPatchId;

    // TODO: Remove
    uint id = inPatchId > 0 ? ~inPatchId : 0;
    float r = float( (id >> 16) & uint(0xFF)) / 255.0f;
	float g = float( (id >> 8)  & uint(0xFF)) / 255.0f;
	float b = float(  id        & uint(0xFF)) / 255.0f;
	vec3 color = vec3(r,g,b);
	outColor = color;
}