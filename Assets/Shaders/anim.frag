#version 400 core

#define MAX_LIGHTS 8

uniform vec4 objectColour;           // For fallback or extra color usage
uniform sampler2D mainTex;           // Diffuse texture
uniform sampler2DShadow shadowTex;   // Shadow map

struct Light {
    vec3 position;
    vec3 direction; 
    vec4 color;
    float radius;
    float cutoff;
};

uniform int numLights;
uniform Light lights[MAX_LIGHTS];

uniform vec3 cameraPos;  // For specular etc.
uniform bool hasTexture; // If we want to sample mainTex

// -- The input block must match your skeleton vertex shader's output block --
in Vertex {
    vec4 colour;
    vec2 texCoord;
    vec4 shadowProj;
    vec3 normal;
    vec3 tangent;
    vec3 binormal;
    vec3 worldPos;
    vec4 worldPosition;
    vec3 position;  // local-space position if you need it
} IN;

// The final output color
out vec4 fragColor;

void main(void)
{
    // 1) Basic shadow logic
    float shadow = 1.0;
    // A bias to reduce shadow acne
    float shadowBias = max(0.005 * (1.0 - dot(IN.normal, normalize(lights[0].position - IN.worldPos))), 0.0005);

    if (IN.shadowProj.w > 0.0) {
        // Use textureProj for shadow mapping
        shadow = textureProj(shadowTex, vec4(IN.shadowProj.xy, IN.shadowProj.z - shadowBias, IN.shadowProj.w));
        // A simple fade: if shadow is near 0, we fade to 0.2, else 1.0
        shadow = mix(0.2, 1.0, shadow);
    }

    // 2) sRGB correction for the vertex color
    // If your vertex color is in linear space, you might skip the pow
    vec4 albedo = pow(IN.colour, vec4(2.2));
    
    // 3) If we have a texture, multiply it in
    if (hasTexture) {
        // sRGB correction for the texture as well
        albedo *= pow(texture(mainTex, IN.texCoord), vec4(2.2));
    }

    // 4) Ambient light base
    vec3 totalLighting = vec3(0.01) * shadow;

    // 5) Multi-light loop
    for (int i = 0; i < numLights; i++) {
        // Light direction & distance
        vec3 L = lights[i].position - IN.worldPos;
        float distance = length(L);
        vec3 lightDir = normalize(L);

        // Basic attenuation
        float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * (distance * distance));

        // Spotlight effect
        float theta = dot(lightDir, normalize(-lights[i].direction));
        float epsilon = lights[i].cutoff - 0.02;
        float spotlightIntensity = clamp((theta - epsilon) / (lights[i].cutoff - epsilon), 0.0, 1.0);

        // Lambertian diffuse
        float lambert = max(dot(lightDir, IN.normal), 0.0);

        // Blinn-Phong specular
        vec3 viewDir = normalize(cameraPos - IN.worldPos);
        vec3 halfDir = normalize(lightDir + viewDir);
        float specular = pow(max(dot(halfDir, IN.normal), 0.0), 32.0);

        // Accumulate
        vec3 lightColor = lights[i].color.rgb;
        totalLighting += (lambert + specular) * lightColor * attenuation * spotlightIntensity * shadow;
    }

    // 6) Combine lighting with albedo, apply gamma correction
    vec3 finalColor = pow(totalLighting * albedo.rgb, vec3(1.0 / 2.2));
    fragColor = vec4(finalColor, albedo.a);
}
