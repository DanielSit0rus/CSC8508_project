#version 400 core

#define MAX_LIGHTS 8

uniform vec4 		objectColour;
uniform sampler2D 	mainTex;
uniform sampler2DShadow shadowTex;



struct Light {
    vec3 position;
    vec3 direction; 
    vec4 color;
    float radius;
    float cutoff;
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
    float shadow = 1.0;
    float shadowBias = max(0.005 * (1.0 - dot(IN.normal, normalize(lights[0].position - IN.worldPos))), 0.0005);;

    if (IN.shadowProj.w > 0.0) {
        shadow = textureProj(shadowTex,  vec4(IN.shadowProj.xy, IN.shadowProj.z - shadowBias, IN.shadowProj.w));
	shadow = mix(0.2, 1.0, shadow);
    }

    vec4 albedo = pow(IN.colour, vec4(2.2));
    if (hasTexture) {
        albedo *=  pow(texture(mainTex, IN.texCoord), vec4(2.2));
    }
    

    vec3 totalLighting = vec3(0.01f) * shadow; // Ambient light, reduced further

	for (int i = 0; i < numLights; i++) {
    		vec3 lightDir = normalize(lights[i].position - IN.worldPos);
    		float distance = length(lights[i].position - IN.worldPos);
    		float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * (distance * distance));

    		// Spotlight effect
    		float theta = dot(lightDir, normalize(-lights[i].direction)); // Compare with light direction
    		float epsilon = lights[i].cutoff - 0.02; // Soft edge cutoff
    		float spotlightIntensity = clamp((theta - epsilon) / (lights[i].cutoff - epsilon), 0.0, 1.0);

    		float lambert = max(0.0, dot(lightDir, IN.normal));
    		vec3 viewDir = normalize(cameraPos - IN.worldPos);
    		vec3 halfDir = normalize(lightDir + viewDir);
    		float specular = pow(max(dot(halfDir, IN.normal), 0.0), 32.0);

    		totalLighting += (lambert + specular) * lights[i].color.rgb * attenuation * spotlightIntensity * shadow;
	}

    fragColor.rgb = pow(totalLighting * albedo.rgb, vec3(1.0f / 2.2f)); // Gamma correction
    fragColor.a = albedo.a;
}