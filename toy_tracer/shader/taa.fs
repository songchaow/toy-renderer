#version 330 core

layout (location = 0) out vec4 hdrColor;

in vec2 TexCoord;

uniform sampler2D currentColor;
uniform sampler2D historyTAAResult;
uniform float weightofHistory = 0.95;

void main() {
      hdrColor = texture(currentColor, TexCoord) * (1-weightofHistory) + texture(historyTAAResult, TexCoord) * weightofHistory;
}
