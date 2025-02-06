#version 400 core
uniform mat4 lightMatrix; // Light-space transformation

layout(location = 0) in vec3 position;

void main(void) {
    gl_Position = lightMatrix * vec4(position, 1.0);
}