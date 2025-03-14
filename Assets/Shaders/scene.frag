#version 400 core

#define MAX_LIGHTS 8

uniform sampler2D       mainTex;        // Diffuse texture
uniform sampler2D       specularTex;    // Specular map (optional)
uniform sampler2D       normalTex;      // Normal map   (optional)
uniform sampler2DShadow shadowTex;

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
    vec3 normal;
    vec3 worldPos;
} IN;

out vec4 fragColor;

void main(void)
{
    // 1) Shadow sampling
    float shadow = 1.0;
    if (IN.shadowProj.w > 0.0) {
        float rawShadow = textureProj(shadowTex, IN.shadowProj);
        // mix(0.4, 1.0, rawShadow) means ~0.4 darkness in fully shadowed areas
        shadow = mix(0.4, 1.0, rawShadow);
    }

    // 2) Base color (diffuse)
    // Convert vertex color to linear space
    vec4 albedo = pow(IN.colour, vec4(2.2));
    if (hasTexture) {
        // Sample diffuse texture, also convert from sRGB -> linear
        albedo *= pow(texture(mainTex, IN.texCoord), vec4(2.2));
    }

    // 3) Normal mapping fallback
    // If normalTex is valid, sample it. Otherwise, fallback to the geometry normal.
    vec3 N;
    if (textureSize(normalTex, 0).x > 1) {
        // Expand normal from [0..1] to [-1..1]
        N = normalize(texture(normalTex, IN.texCoord).rgb * 2.0 - 1.0);
    } else {
        N = normalize(IN.normal);
    }

    // 4) Specular mapping fallback
    float specularStrength = 1.0;
    if (textureSize(specularTex, 0).x > 1) {
        // e.g. Red channel might store spec factor
        specularStrength = texture(specularTex, IN.texCoord).r;
    }

    // 5) Start with a small ambient term
    vec3 result = albedo.rgb * 0.05;

    // 6) Lighting loop
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
        float spec   = pow(max(dot(halfDir, N), 0.0), 32.0) * specularStrength;

        // Distance attenuation
        float attenuation = 1.0 / (1.0 + 0.005 * dist + 0.0001 * (dist * dist));

        // Spotlight check
        if(lights[i].type == 1)
        {
            float angle = dot(lightDir, -normalize(lights[i].direction));
            float cutoffVal = lights[i].cutoff;  // Already cos(angle)
            float eps       = cutoffVal - 0.02; 
            float intensity = clamp((angle - eps)/(cutoffVal - eps), 0.0, 1.0);

            lambert *= intensity;
            spec    *= intensity;
        }

        // Combine diffuse + spec, scaled by (light color * attenuation * shadow)
        vec3 lightCol = lights[i].color.rgb;
        result += (lambert + spec) * lightCol * attenuation * shadow;
    }

    // 7) Final gamma correction (linear -> sRGB)
    result = pow(result, vec3(1.0 / 2.2));
    fragColor = vec4(result, albedo.a);
}
