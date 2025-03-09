#version 400 core

// The mvpMatrix combines the light's projection, view, and the object's model matrix.
uniform mat4 mvpMatrix;

layout(location = 0) in vec3 position;

void main(void) {
    gl_Position = mvpMatrix * vec4(position, 1.0);
}
