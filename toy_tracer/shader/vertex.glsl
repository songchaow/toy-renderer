# version 330 core
layout (location = 0) in vec3 posLocal;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 normalLocal;
layout (location = 3) in vec3 pColor; // deprecated

uniform mat4 obj2world;
uniform mat4 world2cam;
uniform mat4 cam2ndc;

out vec3 ver_color; // deprecated
out vec2 TexCoord;
out vec3 posWorld;
out vec3 normalWorld;

void main()
{
      gl_Position = cam2ndc * world2cam * obj2world * vec4(posLocal.x, posLocal.y, posLocal.z, 1.0);
      posWorld = vec3(obj2world * vec4(posLocal,1.0));
      TexCoord = texCoord;
      ver_color = pColor;
      normalWorld = vec3(transpose(inverse(cam2ndc * world2cam * obj2world)) * vec4(normalLocal,0));
}