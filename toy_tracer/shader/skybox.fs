#version 330 core
in vec3 coord;
uniform samplerCube skybox;
out vec4 FragColor;

void main() {
      gl_FragDepth = 1.0;
      FragColor = vec4(texture(skybox, coord).rgb, 1.0);
}