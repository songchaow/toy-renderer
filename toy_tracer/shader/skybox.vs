#version 330 core
layout (location = 0) in vec3 posWorld;

uniform mat4 rotation;
uniform mat4 cam2ndc;

out vec3 coord;
out vec3 ndcPos;

void main() {
      gl_Position = cam2ndc * rotation * vec4(posWorld, 1.0);
      coord = posWorld;
      //ndcPos = gl_Position.xy / gl_Position.w;
      ndcPos = gl_Position.xyw;
}