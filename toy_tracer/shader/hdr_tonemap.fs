#version 330 core

in vec2 TexCoord;
uniform sampler2D radiance;

void main() {

      vec3 color = texture(radiance, TexCoord).rgb;
      // HDR tonemapping
      color = color / (color + vec3(1.0));
      // gamma correct
      color = pow(color, vec3(1.0/2.2)); 
      gl_FragColor = vec4(color, 1.0);
}