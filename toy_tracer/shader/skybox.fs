#version 330 core
in vec3 coord;
in vec3 ndcPos;
uniform samplerCube skybox;
uniform mat4 rotationPrev;
uniform mat4 cam2ndc;
layout (location = 0) out vec4 FragColor;
layout (location = 2) out vec2 Motion;

void main() {
      //gl_FragDepth = ;
      vec4 position0 = cam2ndc * rotationPrev * vec4(coord, 1.0);
      position0.xy /= position0.w;
      vec2 offset = (position0.xy - ndcPos.xy/ndcPos.z) / 2;
      Motion = offset;
      FragColor = vec4(texture(skybox, coord).rgb, 1.0);
}