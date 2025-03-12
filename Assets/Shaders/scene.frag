#version 400 core

#define MAX_LIGHTS 8

uniform sampler2D       mainTex;
uniform sampler2DShadow shadowTex;

struct Light {
    vec3 position;
    vec3 direction;
    vec4 color;
    float radius;
    float cutoff;
    int   type;  // 0 = point, 1 = spot
};

uniform int numLights;
uniform Light lights[MAX_LIGHTS];

uniform vec3  cameraPos;
uniform bool  hasTexture;

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
    if (IN.shadowProj.w > 0.0) {
        float rawShadow = textureProj(shadowTex, IN.shadowProj);
        shadow = mix(0.4, 1.0, rawShadow);
    }

    // Convert color from sRGB to linear
    vec4 albedo = pow(IN.colour, vec4(2.2));
    if (hasTexture) {
        albedo *= pow(texture(mainTex, IN.texCoord), vec4(2.2));
    }

    // Start with small ambient
    vec3 result = albedo.rgb * 0.01;

    for(int i = 0; i < numLights; i++)
    {
        // Basic lambert & specular
        vec3 L = lights[i].position - IN.worldPos;
        float dist     = length(L);
        vec3 lightDir  = normalize(L);

        float lambert = max(dot(lightDir, IN.normal), 0.0);
        vec3 viewDir  = normalize(cameraPos - IN.worldPos);
        vec3 halfDir  = normalize(lightDir + viewDir);
        float spec    = pow(max(dot(halfDir, IN.normal), 0.0), 32.0);

        // Distance attenuation
        float attenuation = 1.0 / (1.0 + 0.02 * dist + 0.001 * dist*dist);
          // If it's a spotlight, modulate by angle
        if(lights[i].type == 1)
        {
            float angle = dot(lightDir, -normalize(lights[i].direction));
            // lights[i].cutoff is already cos(angle)
            float cutoffVal = lights[i].cutoff;
            float eps       = cutoffVal - 0.02; 
            float intensity = clamp((angle - eps)/(cutoffVal - eps), 0.0, 1.0);

            lambert *= intensity;
            spec    *= intensity;
        }

        // Combine diffuse + spec, scaled by light color, attenuation, and shadow
        vec3 lightCol = lights[i].color.rgb;
        result += (lambert + spec) * lightCol * attenuation * shadow;
    }

    // Final gamma correction
    result = pow(result * albedo.rgb, vec3(1.0/2.2));
    fragColor = vec4(result, albedo.a);
}
