#version 400 core

#define MAX_LIGHTS 8

uniform vec4 		objectColour;
uniform sampler2D 	mainTex;
uniform sampler2DShadow shadowTex;



struct Light {
    vec3 position;
    vec4 color;
    float radius;
};

uniform int numLights;
uniform Light lights[MAX_LIGHTS];

uniform vec3	cameraPos;

uniform bool hasTexture;

in Vertex
{
	vec4 colour;
	vec2 texCoord;
	vec4 shadowProj;
	vec3 normal;
	vec3 worldPos;
} IN;

out vec4 fragColor;

void main(void)
{
	float shadow = 1.0; // New !
	
	if( IN . shadowProj . w > 0.0) { // New !
		shadow = textureProj ( shadowTex , IN . shadowProj ) * 0.5f;
	}

vec4 albedo = IN.colour;
if (hasTexture) {
    albedo *= texture(mainTex, IN.texCoord);
}
albedo.rgb = pow(albedo.rgb, vec3(2.2)); // Apply gamma correction

vec3 totalLighting = vec3(0.05f) * shadow; // Ambient light

for (int i = 0; i < numLights; i++) {
    vec3 incident = normalize(lights[i].position - IN.worldPos);
    float lambert = max(0.0, dot(incident, IN.normal)) * 0.9; 

    vec3 viewDir = normalize(cameraPos - IN.worldPos);
    vec3 halfDir = normalize(incident + viewDir);

    float rFactor = max(0.0, dot(halfDir, IN.normal));
    float sFactor = pow(rFactor, 80.0);

    float attenuation = 1.0 / (1.0 + 0.09 * length(lights[i].position - IN.worldPos));

    totalLighting += albedo.rgb * lights[i].color.rgb * lambert * attenuation;
    totalLighting += lights[i].color.rgb * sFactor * attenuation * shadow;
}

fragColor.rgb = pow(totalLighting, vec3(1.0 / 2.2f)); // Gamma correction
fragColor.a = albedo.a;

//fragColor.rgb = IN.normal;

	//fragColor = IN.colour;
	
	//fragColor.xy = IN.texCoord.xy;
	
	//fragColor = IN.colour;
}