#version 330 core
layout (location = 0) out vec4 FragColor;

uniform sampler2D albedoSampler;

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
} VOutput;

void main() {
    vec4 albedoAlpha = texture(albedoSampler, VOutput.TexCoord);
    if(albedoAlpha.a==0.0)
        discard;
    FragColor = albedoAlpha;
}