#version 400 core

uniform vec4 objectColour;
uniform sampler2D mainTex;
uniform sampler2DShadow shadowTex;

uniform vec3 lightPos;    
uniform vec4 lightColour;

uniform vec3 cameraPos;
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
	
	if(IN.shadowProj.w > 0.0) { 
		float rawShadow = textureProj(shadowTex, IN.shadowProj);
        // mix(valueWhenShadowIs1, valueWhenShadowIs0, rawShadow)
        // When rawShadow = 0, shadow = 0.4
        // When rawShadow = 1, shadow = 1.0
        shadow = mix(0.4, 1.0, rawShadow);
	}

	vec3 incident = normalize(lightPos - IN.worldPos);
	float lambert = max(0.0, dot(incident, IN.normal)) * 0.6; 
	
	vec3 viewDir = normalize(cameraPos - IN.worldPos);
	vec3 halfDir = normalize(incident + viewDir);
	float rFactor = max(0.0, dot(halfDir, IN.normal));
	float sFactor = pow(rFactor, 100000.0);
	
	vec4 albedo = IN.colour;
	
	if (hasTexture) {
		albedo *= texture(mainTex, IN.texCoord);
	}
	
	albedo.rgb = pow(albedo.rgb, vec3(2.2));
	
	fragColor.rgb = albedo.rgb * 0.0099f; // Ambient light
	
	fragColor.rgb += albedo.rgb * lightColour.rgb * lambert * shadow; // Diffuse light
	
	fragColor.rgb += lightColour.rgb * sFactor; // Specular light
	
	fragColor.rgb = pow(fragColor.rgb, vec3(1.0 / 2.2f));
	
	fragColor.a = albedo.a;
}
