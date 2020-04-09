#version 330 core
in vec3 posWorldFs;
in float depth;
in vec2 texCoord;

uniform mat4 world2cam;
uniform float far;
uniform vec3 camPos;
// directional light
uniform bool directional;
uniform vec3 direction;
uniform sampler2D albedoSampler;

void main() {
      float alpha = texture(albedoSampler, texCoord).a;
      if(alpha < 0.1)
            discard;
      // TODO: if using psm, calculate distanceNDC
      // need to pass from geometry shader posNDC
      // or compute difference between gl_Position/z and posLightNDC
      //if (directional)
            //gl_FragDepth = depth;//depth = gl_Position.z;
      // else {
      //       depth = length(posWorldFs - camPos);
      //       depth /= far;
      //       gl_FragDepth = depth;
      // }
      //gl_FragDepth = 0.4;
}