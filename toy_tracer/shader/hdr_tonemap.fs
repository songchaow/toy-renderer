#version 330 core

in vec2 TexCoord;
uniform sampler2D radiance;
uniform sampler2D bloom;
uniform float explosure;

out vec4 fragColor;

void main() {

      vec4 colorAll = texture(radiance, TexCoord);
      vec3 color = colorAll.rgb;
      vec3 bloomColor = texture(bloom, TexCoord).rgb;
      color += bloomColor;
      // HDR tonemapping
      //color = color / (color + vec3(1.0));

      color = vec3(1.0) - exp(-color* explosure);
      // debug
      //color.r = colorAll.a / 200;
      //color.r = max(0, 1 - colorAll.a / 500);
      // gamma correct
      color = pow(color, vec3(1.0/2.2)); 

      fragColor = vec4(color, 1.0);
}