#version 330 core
layout (location = 0) in vec3 posWorld;

uniform mat4 rotation;
uniform mat4 cam2ndc;

out vec3 coord;

void main() {
      gl_Position = vec4(cam2ndc * rotation * posWorld, 1);
      coord = posWorld;
}