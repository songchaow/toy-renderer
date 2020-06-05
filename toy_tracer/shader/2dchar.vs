#version 330 core

// vertex data: 2 fixed size triangles, in xy plane. z = 0
layout (location = 0) in vec3 posLocal;

uniform mat4 obj2world;
uniform mat4 world2cam;
uniform mat4 cam2ndc;

void main() {
    vec2 offset = vec2(posLocal);
    vec4 posCam_org = world2cam * obj2world * vec4(0);
    // the same z value in NDC
    vec4 posNDC_org = cam2ndc * posCam_org; // not clipped now!
    // the ratio

}