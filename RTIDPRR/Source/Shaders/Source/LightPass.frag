#version 450
#extension GL_ARB_separate_shader_objects : enable

#define MAX_LIGHTS 3

layout(location = 0) in vec2 uv;

layout(location = 0) out vec4 outColor;

layout (binding = 0) uniform sampler2D albedoSampler;
layout (binding = 1) uniform sampler2D normalSampler;
layout (binding = 2) uniform sampler2D positionSampler;
layout (binding = 3) uniform sampler2D depthSampler;
layout (binding = 4) uniform LightData {
    mat4 matrix;
    vec4 direction;
    float intensity;
} lightData[MAX_LIGHTS];
layout (binding = 5) uniform sampler2D lightDepthSampler;

layout (push_constant) uniform CameraMatrices {
    mat4 invProjection;
    mat4 invView;
} cameraMatrices;

const float ambientTerm = 0.005f;

const mat4 biasMat = mat4( 
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.5, 0.5, 0.0, 1.0 
);

float textureProj(vec4 shadowCoord, vec2 off) {
	float shadow = 1.0;
	if ( shadowCoord.z > -1.0 && shadowCoord.z < 1.0 ) {
		float dist = texture( lightDepthSampler, vec2(shadowCoord.s, 1 - shadowCoord.t) + off ).r;
		if ( shadowCoord.w > 0.0 && dist  > shadowCoord.z) {
			shadow = ambientTerm;
		}
	}
	return shadow;
}


float filterPCF(vec4 sc) {
	ivec2 texDim = textureSize(lightDepthSampler, 0);
	float scale = 1.5;
	float dx = scale * 1.0 / float(texDim.x);
	float dy = scale * 1.0 / float(texDim.y);

	float shadowFactor = 0.0;
	int count = 0;
	int range = 1;
	
	for (int x = -range; x <= range; x++) {
		for (int y = -range; y <= range; y++) {
			shadowFactor += textureProj(sc, vec2(dx*x, dy*y));
			count++;
		}
	
	}
	return shadowFactor / count;
}

void main() {
    vec3 albedo = texture(albedoSampler, uv).rgb;
    vec3 normal = texture(normalSampler, uv).xyz;
    float depth = texture(depthSampler, uv).x;
    vec3 position = texture(positionSampler, uv).xyz;
    vec3 lightDir = lightData[0].direction.xyz;

    float diffuseTerm = 0.0f;

    vec4 inShadowCoord =  (biasMat * lightData[0].matrix) * vec4(position,1.0f);
	inShadowCoord = inShadowCoord / inShadowCoord.w;
	float shadowTerm = filterPCF(inShadowCoord);

    for (int index = 0; index < MAX_LIGHTS; ++index) {
        float NdotL = clamp(dot(normal, -lightData[index].direction.xyz), 0.0f, 1.0f);
        diffuseTerm += NdotL * lightData[index].intensity;
    }

	outColor = vec4(albedo * (ambientTerm + diffuseTerm * clamp((1.0f - shadowTerm), 0,1)), 1.0f); 
}

