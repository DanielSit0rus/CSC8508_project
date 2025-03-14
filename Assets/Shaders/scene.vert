#version 400 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 shadowMatrix;

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 colour;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in vec3 normal;
layout(location = 4) in vec4 tangent;    
layout(location = 5) in vec4 jointWeights;  // Bone Weights
layout(location = 6) in ivec4 jointIndices; // Bone Indices

uniform mat4 joints[128];  // Joint matrix array
uniform bool hasAnimation;

uniform vec4 objectColour;
uniform bool hasVertexColours;

out Vertex {
    vec4 colour;
    vec2 texCoord;
    vec4 shadowProj;
    vec3 normal;
    vec3 tangent;
    vec3 binormal; // New! Binormal calculated in the vertex shader
    vec3 worldPos;
} OUT;

void main(void)
{
    // Compute Model-View-Projection matrix transformation
    mat4 mvp = (projMatrix * viewMatrix * modelMatrix);
    mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));

    // NON-ANIMATED VALUES
    vec4 shadowProjVal = shadowMatrix * vec4(position, 1.0);
    vec3 worldPosVal = (modelMatrix * vec4(position, 1.0)).xyz;
    vec3 normalVal = normalize(normalMatrix * normal);
    vec3 tangentVal = normalize(normalMatrix * tangent.xyz);

    // ANIMATION PART
    vec4 localPos = vec4(position, 1.0);
    vec4 localNormal = vec4(normal, 0.0);
    vec4 localTangent = vec4(tangent.xyz, 0.0); // Tangent needs animation too

    if (hasAnimation) {
        vec4 skelPos = vec4(0.0);
        vec4 skelNormal = vec4(0.0);
        vec4 skelTangent = vec4(0.0);

        for (int i = 0; i < 4; ++i) {
            int idx = int(jointIndices[i]);  // Convert ivec4 to int
            float weight = jointWeights[i];  // Get joint weight

            skelPos += joints[idx] * localPos * weight;
            skelNormal += joints[idx] * localNormal * weight;
            skelTangent += joints[idx] * localTangent * weight; // Apply animation to tangent
        }

        localPos = skelPos;
        localNormal = skelNormal;
        localTangent = skelTangent;

        vec4 worldSkelPos = modelMatrix * localPos;

        worldPosVal = worldSkelPos.xyz;
        shadowProjVal = shadowMatrix * worldSkelPos;
        normalVal = normalize(normalMatrix * localNormal.xyz);
        tangentVal = normalize(normalMatrix * localTangent.xyz);
    }

    // Compute binormal (cross product of normal and tangent)
    vec3 binormalVal = cross(normalVal, tangentVal) * tangent.w; // * handedness

    // Pass transformed values to the fragment shader
    OUT.shadowProj = shadowProjVal;
    OUT.worldPos = worldPosVal;
    OUT.normal = normalVal;
    OUT.tangent = tangentVal; // Pass tangent
    OUT.binormal = binormalVal; // Pass binormal

    OUT.texCoord = texCoord;
    OUT.colour = objectColour;

    if (hasVertexColours) {
        OUT.colour = objectColour * colour;
    }

    // Final position of vertex
    gl_Position = mvp * localPos;
}
