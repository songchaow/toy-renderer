#version 330 core

in vec2 TexCoord;
uniform sampler2D image;

out vec4 fragColor;

void main() {

      vec3 color = texture(image, TexCoord).rgb;
      fragColor = vec4(color, 1.0);
}