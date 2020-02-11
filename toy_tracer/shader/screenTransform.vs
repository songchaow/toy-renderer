# version 330 core
layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 texCoord;

uniform ivec2 screenSize;
out vec2 TexCoord;

void main() {
      float x = 2 * pos.x / 1280.0 - 1;
      float y = 2 * pos.y / 800.0 - 1;
      gl_Position = vec4(x, y, 0, 1);
      TexCoord = texCoord;
}