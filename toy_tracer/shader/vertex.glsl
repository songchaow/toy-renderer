# version 330 core
layout (location = 0) in vec3 posLocal;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 normalLocal;
layout (location = 3) in vec4 tangentLocal;

uniform mat4 obj2world;
uniform mat4 world2cam;
uniform mat4 cam2ndc;

out vec2 TexCoord;
out vec3 posWorld;
out vec3 normalWorld;
out vec4 tangentWorld;
out float zValuePos;


void main()
{
      posWorld = vec3(obj2world * vec4(posLocal,1.0));
      vec3 posCam = vec3(world2cam * vec4(posWorld, 1.0));
      zValuePos = -posCam.z;
      gl_Position = cam2ndc * vec4(posCam, 1.0);
      TexCoord = texCoord;
      tangentWorld = vec4((obj2world * vec4(tangentLocal.xyz, 0)).xyz, tangentLocal.w);
      normalWorld = vec3(transpose(inverse(obj2world)) * vec4(normalLocal,0));
}