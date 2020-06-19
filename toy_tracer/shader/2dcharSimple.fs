#version 330 core
layout (location = 0) out vec4 FragColor;

uniform sampler2D albedoSampler;
// uniform bool forceDepth;
// uniform float depth;

void main() {
    vec4 albedoAlpha = texture(albedoSampler, VOutput.TexCoord);
    if(albedoAlpha.a==0.0)
        discard;
    FragColor = albedoAlpha;
}
