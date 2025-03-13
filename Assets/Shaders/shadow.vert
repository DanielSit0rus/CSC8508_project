#version 400 core

uniform mat4 mvpMatrix;     // LightProjection * LightView * modelMatrix
uniform mat4 joints[128];   // Each = (frameData[j] * invBindPose[j]) from CPU
uniform bool hasAnimation;

layout(location = 0) in vec3 position;
layout(location = 5) in vec4 jointWeights;
layout(location = 6) in ivec4 jointIndices;

void main(void) {
    // Start from local/bind-pose position
    vec4 localPos = vec4(position, 1.0);

    if (hasAnimation) {
        vec4 skinnedPos = vec4(0.0);
        for(int i = 0; i < 4; i++) {
            int   idx = jointIndices[i];
            float w   = jointWeights[i];
            // Weighted bone transform:
            skinnedPos += (joints[idx] * localPos) * w;
        }
        localPos = skinnedPos;
    }
    gl_Position = mvpMatrix * localPos;
}
