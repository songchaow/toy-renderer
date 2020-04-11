#version 330 core
in vec2 TexCoord;
out vec3 FragColor;

uniform sampler2D firstImg;
uniform sampler2D secondImg;

void main() {
      vec3 firstColor = texture(firstImg, TexCoord).rgb;
      vec3 secondColor = texture(secondImg, TexCoord).rgb;
      vec3 diff = abs(firstColor - secondColor) * 2;
      FragColor = diff;
}