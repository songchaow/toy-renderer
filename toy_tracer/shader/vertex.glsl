# version 330 core
layout (location = 0) in vec3 posLocal;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 normalLocal;
layout (location = 3) in vec3 pColor; // deprecated

struct PointLight {
    vec3 pos;
    // vec3 diffuse;
    // vec3 specular;
    // vec3 ambient;
    vec3 irradiance;
    bool directional;
    vec3 direction;
    float cosAngle;

};

uniform mat4 obj2world;
uniform mat4 world2cam;
uniform mat4 cam2ndc;
#define POINT_LIGHT_NUM 4
uniform PointLight pointLights[POINT_LIGHT_NUM];

out vec3 ver_color; // deprecated
out vec2 TexCoord;
out vec3 posWorld;
out vec3 normalWorld;
out float depth[POINT_LIGHT_NUM];




void main()
{
      gl_Position = cam2ndc * world2cam * obj2world * vec4(posLocal.x, posLocal.y, posLocal.z, 1.0);
      posWorld = vec3(obj2world * vec4(posLocal,1.0));
      TexCoord = texCoord;
      ver_color = pColor;
      normalWorld = vec3(transpose(inverse(obj2world)) * vec4(normalLocal,0));
      for(int i=0;i<POINT_LIGHT_NUM;i++)
        depth[i] = length(pointLights[i].pos - posWorld);
}