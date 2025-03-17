#version 400 core

#define MAX_LIGHTS 8

// Texture Samplers
uniform sampler2D       mainTex;        // Diffuse texture
uniform sampler2D       specularTex;    // Specular map (optional)
uniform sampler2D       normalTex;      // Normal map   (optional)
uniform sampler2D       heightTex;      // Height map   (optional)
uniform sampler2D       metallicTex;    // Metallic map (optional)
uniform sampler2D       occlusionTex;   // Occlusion map (optional)
uniform sampler2D       maskMapTex;     // Mask map (optional)
uniform sampler2D       roughnessTex;   // Roughness map (optional)
uniform sampler2DShadow shadowTex;      // Shadow map

struct Light {
    vec3 position;
    vec3 direction;
    vec4 color;
    float radius;
    float cutoff;
    int   type;  // 0 = point, 1 = spot
};

uniform int  numLights;
uniform Light lights[MAX_LIGHTS];

uniform vec3  cameraPos;
uniform bool  hasTexture;

in Vertex {
    vec4 colour;        // Possibly vertex color
    vec2 texCoord;
    vec4 shadowProj;    // For sampling shadow
    vec3 normal;        // Normal from vertex shader
    vec3 tangent;       // Tangent from vertex shader
    vec3 binormal;      // Binormal from vertex shader
    vec3 worldPos;
} IN;

out vec4 fragColor;

void main(void)
{
    // 1) Shadow sampling
    float shadow = 1.0;
    if (IN.shadowProj.w > 0.0) {
        float rawShadow = textureProj(shadowTex, IN.shadowProj);
        shadow = mix(0.4, 1.0, rawShadow);
    }

    // 2) Base color (diffuse)
    vec4 albedo = pow(IN.colour, vec4(2.2));  // Convert vertex color to linear space
    if (hasTexture) {
        albedo *= pow(texture(mainTex, IN.texCoord), vec4(2.2)); // Convert from sRGB -> linear
    }

    // 3) Construct TBN Matrix for Normal Mapping
    mat3 TBN = mat3(normalize(IN.tangent), normalize(IN.binormal), normalize(IN.normal));

    // 4) Normal mapping fallback
    vec3 N = normalize(IN.normal);
    if (textureSize(normalTex, 0).x > 1) {
        N = normalize(TBN * (texture(normalTex, IN.texCoord).rgb * 2.0 - 1.0));
    } 

    // 5) Specular Mapping
    float specularStrength = 0.0;
    if (textureSize(specularTex, 0).x > 1) {
        specularStrength = texture(specularTex, IN.texCoord).r;
    }

    // 6) Metallic Mapping (default = 0.0)
    float metallic = 0.0;
    if (textureSize(metallicTex, 0).x > 1) {
        metallic = texture(metallicTex, IN.texCoord).r;
    }

    // 7) Roughness Mapping (default = 1.0 for smooth surfaces)
    float roughness = 1.0;
    if (textureSize(roughnessTex, 0).x > 1) {
        roughness = texture(roughnessTex, IN.texCoord).r;
    }

    // 8) Ambient Occlusion Mapping (default = 1.0 for no occlusion)
    float ao = 1.0;
    if (textureSize(occlusionTex, 0).x > 1) {
        ao = texture(occlusionTex, IN.texCoord).r;
    }

    // 9) Mask Map Mapping (default = white)
    vec4 maskMap = vec4(1.0);
    if (textureSize(maskMapTex, 0).x > 1) {
        maskMap = texture(maskMapTex, IN.texCoord);
    }

    // 10) Start with a small ambient term
    vec3 result = albedo.rgb * 0.005 * ao; // Multiply by ambient occlusion

    // 11) Lighting loop
    for(int i = 0; i < numLights; i++)
    {
        vec3 L = lights[i].position - IN.worldPos;
        float dist    = length(L);
        vec3 lightDir = normalize(L);

        // Diffuse (Lambert)
        float lambert = max(dot(lightDir, N), 0.0);

        // Specular (Blinn-Phong)
        vec3 viewDir = normalize(cameraPos - IN.worldPos);
        vec3 halfDir = normalize(lightDir + viewDir);
        float spec   = pow(max(dot(halfDir, N), 0.0), 1.0) * specularStrength * (1.0 - roughness);

        // Distance attenuation
        float attenuation = 1.0 / (1.0 + 0.005 * dist + 0.0001 * (dist * dist));

        // Spotlight check
        if(lights[i].type == 1)
        {
            float angle = dot(lightDir, -normalize(lights[i].direction));
            float cutoffVal = lights[i].cutoff;
            float eps       = cutoffVal - 0.02; 
            float intensity = clamp((angle - eps)/(cutoffVal - eps), 0.0, 1.0);

            lambert *= intensity;
            spec    *= intensity;
        }

        // Combine diffuse + specular + metallic, scaled by (light color * attenuation * shadow)
        vec3 lightCol = lights[i].color.rgb;
        result += (lambert * albedo.rgb * (1.0 - metallic)) * lightCol * attenuation * shadow;
        result += (spec * albedo.rgb) * lightCol * attenuation * shadow * 0.5; // Specular scaled
    }

    // 12) Final gamma correction (linear -> sRGB)
    result = pow(result, vec3(1.0 / 2.2));
    fragColor = vec4(result, albedo.a);
}
