#version 430 core
layout (location = 0) out vec4 FragColor;

uniform sampler2D albedoSampler;
uniform usampler2D objectID;

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
    // totally occluded
    if(occlusionInfo.hasOccluded[id] && !occlusionInfo.hasUnoccluded[id])
        discard;
    // if(occlusionInfo.hasUnoccluded[id])
    //     gl_FragDepth = float(occlusionInfo.near[id]-1) / 0xffffffff;
    // else
    //     gl_FragDepth = float(occlusionInfo.far[id]+1) / 0xffffffff;
    FragColor = albedoAlpha;
}