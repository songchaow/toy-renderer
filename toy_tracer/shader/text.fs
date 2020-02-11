# version 330 core
in vec2 TexCoord;
out vec4 FragColor;
uniform sampler2D text;

void main() {
      if(texture(text, TexCoord).r < 0.1)
            discard;
      FragColor = vec4(1.0, 1.0, 1.0, texture(text, TexCoord).r);
}