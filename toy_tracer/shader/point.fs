#version 330 core
uniform vec3 color = vec3(1,0,0);

layout (location = 0) out vec4 FragColor;

void main() {
      FragColor = vec4(color,1);
}