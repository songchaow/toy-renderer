#version 330 core
in vec3 posWorldFs;
in float depth;

uniform mat4 world2cam;
uniform float far;
uniform vec3 camPos;
// directional light
uniform bool directional;
uniform vec3 direction;

void main() {
      //if (directional)
            //gl_FragDepth = depth;//depth = gl_Position.z;
      // else {
      //       depth = length(posWorldFs - camPos);
      //       depth /= far;
      //       gl_FragDepth = depth;
      // }
      //gl_FragDepth = 0.4;
}