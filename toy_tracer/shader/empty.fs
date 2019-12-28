#version 330 core
in vec3 faceNormal;
out vec4 FragColor;

void main()
{
      FragColor = vec4(faceNormal, 1.0);
}