#version 330 core
in vec3 posWorld;

uniform mat4 world2cam;
uniform float far;

void main() {
      vec3 camPos = vec3(-world2cam[0][3], -world2cam[1][3], -world2cam[2][3]);
      gl_FragDepth = length(posWorld-camPos) / f;
}