#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_KHR_vulkan_glsl : enable

layout (constant_id = 0) const uint MAX_LIGHT_COUNT = 1;

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
} lightData[MAX_LIGHT_COUNT];
layout (binding = 5) uniform sampler2D lightDepthSamplers[MAX_LIGHT_COUNT];

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

float textureProj(vec4 shadowCoord, vec2 off, sampler2D lightDepthTex) {
	float shadow = 0.0;
	if ( shadowCoord.z > -1.0 && shadowCoord.z < 1.0 ) {
		float dist = texture( lightDepthTex, vec2(shadowCoord.s, 1 - shadowCoord.t) + off ).r;
		if ( shadowCoord.w <= 0.0 || dist <= shadowCoord.z) {
			shadow = 1.0f;
		}
	}
	return shadow;
}


float filterPCF(vec4 sc, sampler2D lightDepthTex) {
	ivec2 texDim = textureSize(lightDepthTex, 0);
	float scale = 1.5;
	float dx = scale * 1.0 / float(texDim.x);
	float dy = scale * 1.0 / float(texDim.y);

	float shadowFactor = 0.0;
	int count = 0;
	int range = 1;
	
	for (int x = -range; x <= range; x++) {
		for (int y = -range; y <= range; y++) {
			shadowFactor += textureProj(sc, vec2(dx*x, dy*y), lightDepthTex);
			count++;
		}
	
	}
	return shadowFactor / count;
}

vec2 poissonDisk[4] = vec2[](
  vec2( -0.94201624, -0.39906216 ),
  vec2( 0.94558609, -0.76890725 ),
  vec2( -0.094184101, -0.92938870 ),
  vec2( 0.34495938, 0.29387760 )
);

float filterPoisson(vec4 sc, sampler2D lightDepthTex) {
	float visibility = 0.0f;
	for (int i=0;i<4;i++) {
		vec2 offset = poissonDisk[i]/1000.0;
		visibility += 0.2f * textureProj(sc, offset, lightDepthTex);
	}
	return visibility;
}


void main() {
    vec3 albedo = texture(albedoSampler, uv).rgb;
    vec3 normal = texture(normalSampler, uv).xyz;
    float depth = texture(depthSampler, uv).x;
    vec3 position = texture(positionSampler, uv).xyz;

	// Compute shadow term
	float shadowTerm = 0.0f;
	for (int index = 0; index < MAX_LIGHT_COUNT; ++index) {
		if (lightData[index].intensity > 0.0f) {
			vec3 lightDir = lightData[index].direction.xyz;
			vec4 inShadowCoord =  (biasMat * lightData[index].matrix) * vec4(position,1.0f);
			inShadowCoord = inShadowCoord / inShadowCoord.w;
			shadowTerm += filterPoisson(inShadowCoord, lightDepthSamplers[index]);
		}
	}
	shadowTerm = clamp((1.0f - shadowTerm), 0.0f, 1.0f);

	// Compute diffuse term
	float diffuseTerm = 0.0f;
    for (int index = 0; index < MAX_LIGHT_COUNT; ++index) {
		float intensity = lightData[index].intensity;
		if (intensity > 0.0f) {
			float NdotL = clamp(dot(normal, -lightData[index].direction.xyz), 0.0f, 1.0f);
			diffuseTerm += NdotL * intensity;
		}
    }

	outColor = vec4(albedo * (ambientTerm + diffuseTerm * shadowTerm), 1.0f); 
}

