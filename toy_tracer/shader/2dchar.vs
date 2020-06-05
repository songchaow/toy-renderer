#version 330 core

// vertex data: 2 fixed size triangles, in xy plane. z = 0
//layout (location = 0) in vec3 posLocal;
//layout (location = 2) in vec2 uvStart;

uniform mat4 obj2world;
uniform mat4 world2cam;
uniform mat4 cam2ndc;
uniform vec2 size;

out VertexOutput {
    vec2 sizeNDC;
} VOutput;

void main() {
    vec4 posCam_org = world2cam * obj2world * vec4(0,0,0,1);
    vec4 posCam_rightUp = posCam_org + vec4(size,0,0);
    // the same z value in NDC
    vec4 posNDC_org = cam2ndc * posCam_org; // not clipped now!
    vec4 posNDC_rightUp = cam2ndc * posCam_rightUp;
    gl_Position = posNDC_org;
    //VOutput.sizeNDC = (posNDC_rightUp - posNDC_org).xy / posNDC_org.w;
    VOutput.sizeNDC = (posNDC_rightUp - posNDC_org).xy;
}