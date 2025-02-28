#version 400 core
#define MAX_LIGHTS 8

uniform mat4 modelMatrix 	= mat4(1.0f);
uniform mat4 viewMatrix 	= mat4(1.0f);
uniform mat4 projMatrix 	= mat4(1.0f);
uniform mat4 shadowMatrix 	= mat4(1.0f);

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 colour;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in vec3 normal;
layout(location = 4) in ivec4 jointIndices; 
layout(location = 5) in vec4 jointWeights;


uniform mat4 joints[128];   // Skeletal joint transformation matrices
uniform bool hasAnimation;
uniform vec4 objectColour = vec4(1,1,1,1);

uniform bool hasVertexColours = false;

out Vertex
{
	vec4 colour;
	vec2 texCoord;
	vec4 shadowProj;
	vec3 normal;
	vec3 worldPos;
} OUT;

void main(void)
{
	vec4 skelPos = vec4(0.0);
	vec4 localPos = vec4(position, 1.0);
	
	if (hasAnimation) {
		float weightSum =  dot(jointWeights, vec4(1.0));
		if (weightSum > 0.0001) {
			for (int i = 0; i < 4; ++i) {
				int jointIndex = clamp(jointIndices[i], 0, 127);
				skelPos += (joints[jointIndex] * localPos) * jointWeights[i];
			}
			skelPos /= weightSum;
		}
		skelPos.w = 1.0;

	} else {
		
		skelPos = localPos;
	}

	vec4 worldSkelPos = modelMatrix * skelPos;
	gl_Position = projMatrix * viewMatrix * worldSkelPos;
	
	OUT.shadowProj 	=  shadowMatrix * worldSkelPos;
	vec3 skinnedNormal = vec3(0.0);
	
	if (hasAnimation) {
		float normalWeightSum = dot(jointWeights, vec4(1.0));

		if (normalWeightSum > 0.0001) {
    			for (int i = 0; i < 4; ++i) {
        			int jointIndex = clamp(jointIndices[i], 0, 127);

        			skinnedNormal += mat3(joints[jointIndex]) * normal * jointWeights[i];
    			}
		//skinnedNormal /= normalWeightSum;
		}
		else{

		skinnedNormal = normal;
	}
	
	skinnedNormal = normalize(mat3(modelMatrix) * skinnedNormal);	
	} else {

		mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));
		skinnedNormal = normalize(normalMatrix * normal);
	}

	OUT.normal = skinnedNormal;
	OUT.worldPos = worldSkelPos.xyz;
	OUT.texCoord	= texCoord;
	OUT.colour = clamp(objectColour, vec4(0.1, 0.1, 0.1, 1.0), vec4(1.0, 1.0, 1.0, 1.0));

	if(hasVertexColours) {
		OUT.colour		= objectColour * colour;
	}
	
}
