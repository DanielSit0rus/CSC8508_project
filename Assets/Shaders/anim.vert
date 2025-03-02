#version 400 core
#define MAX_LIGHTS 8

uniform mat4 modelMatrix    = mat4(1.0f);
uniform mat4 viewMatrix     = mat4(1.0f);
uniform mat4 projMatrix     = mat4(1.0f);
uniform mat4 shadowMatrix   = mat4(1.0f);

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 colour;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in vec3 normal;
layout(location = 4) in ivec4 jointIndices; 
layout(location = 5) in vec4 jointWeights;

uniform mat4 joints[128];   // Skeletal joint transformation matrices
uniform bool hasAnimation;
uniform vec4 objectColour   = vec4(1,1,1,1);
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
    vec4 localPos = vec4(position, 1.0);
    vec4 skelPos;
    vec4 skelNormal;
    
    if(hasAnimation) {
        // Accumulate skinned position and normal (friend's working logic)
        skelPos    = vec4(0.0);
        skelNormal = vec4(0.0);
        for (int i = 0; i < 4; ++i) {
            // Use jointIndices directly (no clamping)
            int j = jointIndices[i];
            float w = jointWeights[i];
            skelPos    += joints[j] * localPos * w;
            skelNormal += joints[j] * vec4(normal, 0.0) * w;
        }
        skelPos.w = 1.0;
    } else {
        skelPos = localPos;
        skelNormal = vec4(normal, 0.0);
    }
    
    vec4 worldSkelPos = modelMatrix * skelPos;
    gl_Position = projMatrix * viewMatrix * worldSkelPos;
    
    OUT.shadowProj = shadowMatrix * worldSkelPos;
    OUT.worldPos   = worldSkelPos.xyz;
    OUT.texCoord   = texCoord;
    
    // Transform the accumulated normal using the inverse-transpose of the modelMatrix.
    mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));
    vec3 finalNormal = normalize(normalMatrix * skelNormal.xyz);
    OUT.normal = finalNormal;
    
    OUT.colour = clamp(objectColour, vec4(0.1,0.1,0.1,1.0), vec4(1.0));
    if(hasVertexColours) {
        OUT.colour *= colour;
    }
}
