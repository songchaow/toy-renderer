#version 430 core
//layout (location = 0) out vec4 FragColor;
uniform sampler2D albedoSampler;
uniform usampler2D objectID;
uniform sampler2D depth3D;

#define NUM_2DOBJ 1
#define NUM_OBJID 16
layout(binding = 0, std140) buffer ObjectOcclusionInfo {
    bool hasOccluded[NUM_2DOBJ * NUM_OBJID]; // 3d obj is partially in front of 2d obj
    bool hasUnoccluded[NUM_2DOBJ * NUM_OBJID]; // 3d obj is partially behind 2d obj
    // uint far[NUM_2DOBJ * NUM_OBJID];
    // uint near[NUM_2DOBJ * NUM_OBJID];
} occlusionInfo;

// #ifdef WITH_GEOMETRY
// in GeometryOutput {
//     vec2 TexCoord;
// } VOutput;
// #else
// in VertexOutput {
//     vec2 TexCoord;
// } VOutput;
// #endif

in GeometryOutput {
    vec2 TexCoord;
    vec2 screenUV;
} VOutput;

void main() {
    vec4 albedoAlpha = texture(albedoSampler, VOutput.TexCoord);
    if(albedoAlpha.a==0.0)
        discard;
    uint id = uint(texture(objectID, VOutput.screenUV).r);
    float depth3dObj = texture(depth3D, VOutput.screenUV).r;
    uint depth3dInt = uint(depth3dObj * 0xffffffff);
    //atomicMax(occlusionInfo.far[id], depth3dInt);
    //atomicMin(occlusionInfo.near[id], depth3dInt);
    if(gl_FragCoord.z >  depth3dObj) {
        occlusionInfo.hasOccluded[id] = true;
        //FragColor = vec4(depth3dObj,0,0,1);
    }
    else {
        occlusionInfo.hasUnoccluded[id] = true;
        //FragColor = vec4(0,1,0,1);
    }

}