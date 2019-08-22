# version 330 core
out vec4 FragColor;

in vec3 ver_color;
in vec2 TexCoord;
in vec3 posWorld;

uniform sampler2D texture1;
uniform sampler2D texture2;


void main()
{
      FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2);
}