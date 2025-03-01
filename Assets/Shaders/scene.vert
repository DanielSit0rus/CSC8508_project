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
	vec4 worldPos = modelMatrix * vec4(position, 1.0);
	gl_Position = projMatrix * viewMatrix * worldPos;
	
	OUT.shadowProj 	= shadowMatrix * worldPos;
	OUT.worldPos 	= worldPos.xyz;
	OUT.texCoord	= texCoord;
	OUT.normal		= normalize(mat3(modelMatrix) * normal);
	OUT.colour		= clamp(objectColour, vec4(0.1, 0.1, 0.1, 1.0), vec4(1.0, 1.0, 1.0, 1.0));

	if (hasVertexColours) {
		OUT.colour = objectColour * colour;
	}
}