# version 330 core
layout (location = 0) in vec3 posLocal;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 normalLocal;
layout (location = 3) in vec4 tangentLocal;
// instanced attribute
layout (location = 4) in mat4 obj2world;

//uniform mat4 obj2world;
uniform mat4 world2cam;
uniform mat4 cam2ndc;

out vec2 TexCoord;
out vec3 posWorld;
out vec3 normalWorld;
out vec4 tangentWorld;



void main()
{
      gl_Position = obj2world * vec4(posLocal.x, posLocal.y, posLocal.z, 1.0);
      posWorld = vec3(obj2world * vec4(posLocal,1.0));
      TexCoord = texCoord;
      tangentWorld = vec4((obj2world * vec4(tangentLocal.xyz, 0)).xyz, tangentLocal.w);
      normalWorld = vec3(transpose(inverse(obj2world)) * vec4(normalLocal,0));
}