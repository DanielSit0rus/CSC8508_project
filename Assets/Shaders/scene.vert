#version 400 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 shadowMatrix;

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 colour;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in vec3 normal;
layout(location = 4) in vec4  tangent;    
layout(location = 5) in vec4 jointWeights;  // Bone Weights
layout(location = 6) in ivec4 jointIndices; // Bone Indices

uniform mat4 joints[128];  // Joint matrix array
uniform bool hasAnimation; // Missing before, now declared properly

uniform vec4 objectColour;
uniform bool hasVertexColours;

out Vertex {
    vec4 colour;
    vec2 texCoord;
    vec4 shadowProj;
    vec3 normal;
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

    // ANIMATION PART
    vec4 localPos = vec4(position, 1.0);
    vec4 localNormal = vec4(normal, 0.0);

    if (hasAnimation) {
        vec4 skelPos =  vec4(0.0);
        vec4 skelNormal =  vec4(0.0);

        for (int i = 0; i < 4; ++i) {
            int idx = int(jointIndices[i]);  // Convert ivec4 to int
            float weight = jointWeights[i];  // Get joint weight

            skelPos += joints[idx] * localPos * weight;
            skelNormal += joints[idx] * localNormal * weight;
        }

        localPos = skelPos;  // localPos is skelPos
        localNormal = skelNormal;  // localNormal is skelNormal

        vec4 worldSkelPos = modelMatrix * localPos;

        worldPosVal = (modelMatrix * localPos).xyz;
        shadowProjVal = shadowMatrix * worldSkelPos;
        normalVal = normalize(normalMatrix * localNormal.xyz);
    }

    // Compute transformed values to pass to the fragment shader
    OUT.shadowProj = shadowProjVal;
    OUT.worldPos = worldPosVal;
    OUT.normal = normalVal;

    OUT.texCoord = texCoord;
    OUT.colour = objectColour;

    if (hasVertexColours) {
        OUT.colour = objectColour * colour;
    }

    // Final position of vertex
    gl_Position = mvp * localPos;
}
