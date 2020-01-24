#version 330 core
in vec3 posWorld;

uniform mat4 world2cam;
uniform float far;
uniform vec3 camPos;

void main() {
      float depth = length(posWorld-camPos);
      depth /= far;
      gl_FragDepth = depth;
      //gl_FragDepth = 0.4;
}