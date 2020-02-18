#version 330 core
in vec3 posWorldFs;

uniform mat4 world2cam;
uniform float far;
uniform vec3 camPos;
// directional light
uniform bool directional;
uniform vec3 direction;

void main() {
      float depth;
      if (directional)
            depth = length(dot(posWorldFs - camPos, direction));
      else
            depth = length(posWorldFs - camPos);
      depth /= far;
      gl_FragDepth = depth;
      //gl_FragDepth = 0.4;
}