#version 330 core
layout (location = 0) in vec2 posNDC;
layout (location = 1) in vec2 texcoord;

out vec2 TexCoord;

void main() {
      gl_Position = vec4(posNDC, 0.0, 1.0);
      TexCoord = texcoord;
}