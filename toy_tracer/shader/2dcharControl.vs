#version 430 core

// vertex data: 2 fixed size triangles, in xy plane. z = 0
//layout (location = 0) in vec3 posLocal;
//layout (location = 2) in vec2 uvStart;

uniform mat4 obj2world;
uniform mat4 world2cam;
uniform mat4 cam2ndc;
uniform vec2 size;
uniform bool forceDepth = false;
uniform float depthCam;

out VertexOutput {
    vec2 sizeNDC;
} VOutput;

void main() {
    vec4 posCam_org = world2cam * obj2world * vec4(0,0,0,1);
    vec4 referenceP = vec4(0,0,posCam_org.z,1);
    if(forceDepth)
        referenceP.z = depthCam;
    vec4 posCam_rightUp = posCam_org + vec4(size,0,0);

    vec4 posNDC_org = cam2ndc * posCam_org; // not clipped now!
    vec4 posNDC_rightUp = cam2ndc * posCam_rightUp;

    // calc z value in NDC
    vec4 referenceNDC = cam2ndc * referenceP;
    
    gl_Position = posNDC_org;
    gl_Position.z = referenceNDC.z / referenceNDC.w * posNDC_org.w;
    //VOutput.sizeNDC = (posNDC_rightUp - posNDC_org).xy / posNDC_org.w;
    VOutput.sizeNDC = (posNDC_rightUp - posNDC_org).xy;
}