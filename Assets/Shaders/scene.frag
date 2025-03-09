#version 400 core

#define MAX_LIGHTS 8

uniform vec4 objectColour;
uniform sampler2D mainTex;
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

float GetShadowFactor(vec4 shadowCoord) {
    vec3 projCoords = shadowCoord.xyz / shadowCoord.w;
    projCoords = projCoords * 0.5 + 0.5;

    float bias = max(0.005 * (1.0 - dot(IN.normal, normalize(lights[0].position - IN.worldPos))), 0.0005);
    float shadowSample = textureProj(shadowTex, vec4(projCoords.xy, projCoords.z - bias, 1.0));

    return mix(0.2, 1.0, shadowSample);
}

void main(void) {
    float shadowFactor = 1.0;
    if (IN.shadowProj.w > 0.0) {
        shadowFactor = GetShadowFactor(IN.shadowProj);
    }

    vec4 albedo = pow(IN.colour, vec4(2.2));
    if (hasTexture) {
        albedo *= pow(texture(mainTex, IN.texCoord), vec4(2.2));
    }

    vec3 totalLighting = vec3(0.001f);

    for (int i = 0; i < numLights; i++) {
        vec3 lightDir = normalize(lights[i].position - IN.worldPos);
        float distance = length(lights[i].position - IN.worldPos);
        float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * (distance * distance));

        float theta = dot(lightDir, normalize(-lights[i].direction));
        float epsilon = lights[i].cutoff - 0.02;
        float spotlightIntensity = clamp((theta - epsilon) / (lights[i].cutoff - epsilon), 0.0, 1.0);

        float lambert = max(0.0, dot(lightDir, IN.normal));
        vec3 viewDir = normalize(cameraPos - IN.worldPos);
        vec3 halfDir = normalize(lightDir + viewDir);
        float specular = pow(max(dot(halfDir, IN.normal), 0.0), 32.0);

        totalLighting += (lambert + specular) * lights[i].color.rgb * attenuation * spotlightIntensity * shadowFactor;
    }

    totalLighting = max(totalLighting, vec3(0.0f));

    fragColor.rgb = pow(totalLighting * albedo.rgb, vec3(1.0f / 2.2f));
    fragColor.a = albedo.a;
}
