#version 400 core

uniform mat4 modelMatrix   = mat4(1.0f);
uniform mat4 viewMatrix    = mat4(1.0f);
uniform mat4 projMatrix    = mat4(1.0f);
uniform mat4 shadowMatrix  = mat4(1.0f);

layout(location = 0) in vec3 position;     
layout(location = 1) in vec4 colour;       
layout(location = 2) in vec2 texCoord;     
layout(location = 3) in vec3 normal;       
layout(location = 4) in vec4 tangent;      
layout(location = 5) in vec4 jointWeights;
layout(location = 6) in ivec4 jointIndices;

uniform mat4 joints[128]; 
uniform vec4 objectColour = vec4(1,1,1,1);
uniform bool hasVertexColours = false;

out Vertex {
    vec4 colour;         
    vec2 texCoord;       
    vec4 shadowProj;    
    vec3 normal;         
    vec3 tangent;        
    vec3 binormal;       
    vec3 worldPos;      
    vec4 worldPosition;  
    vec3 position;       
} OUT;

void main(void)
{

    OUT.texCoord  = texCoord;
    OUT.position  = position;

    mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));

    vec4 localPos    = vec4(position, 1.0f);
    vec4 localNormal = vec4(normal,   0.0f); // 0.0 w means direction
    vec4 localTangent= tangent;             // tangent + sign in w


    vec4 skelPos    = vec4(0.0);
    vec4 skelNormal = vec4(0.0);
    for(int i = 0; i < 4; ++i) {
        int   idx = jointIndices[i];  // we assume valid range
        float w   = jointWeights[i];
        skelPos    += joints[idx] * localPos    * w;
        skelNormal += joints[idx] * localNormal * w;
    }
   
    vec4 skelTangent = vec4(0.0);
    for(int i = 0; i < 4; ++i) {
        int   idx = jointIndices[i];
        float w   = jointWeights[i];
        skelTangent += joints[idx] * localTangent * w;
    }
    vec4 worldSkelPos = modelMatrix * skelPos;

    mat4 mvp   = projMatrix * viewMatrix * modelMatrix;
    gl_Position= mvp * skelPos;

    // Store world-space positions for e.g. shadow sampling, lighting, etc.
    OUT.worldPos      = (modelMatrix * skelPos).xyz; // or worldSkelPos.xyz
    OUT.worldPosition = modelMatrix * skelPos;       // same but in vec4
    OUT.shadowProj    = shadowMatrix * worldSkelPos; // for shadow sampling

    vec3 finalNormal   = normalize(normalMatrix * skelNormal.xyz);
    vec3 finalTangent  = normalize(normalMatrix * skelTangent.xyz);

    float sign = localTangent.w;
    vec3 finalBinormal = cross(finalTangent, finalNormal) * sign;

    OUT.normal   = finalNormal;
    OUT.tangent  = finalTangent;
    OUT.binormal = finalBinormal;


    vec4 baseColour = objectColour;
    if(hasVertexColours) {
        baseColour *= colour;
    }
    OUT.colour = baseColour;
}
