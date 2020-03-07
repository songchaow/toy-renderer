#version 330 core

layout (location = 0) out vec4 hdrColor;

in vec2 TexCoord;

uniform sampler2D currentColor;
uniform sampler2D historyTAAResult;
uniform sampler2D motionVector;
uniform float weightofHistory = 0.95;

void main() {
      vec2 lastUV = TexCoord + texture(motionVector, TexCoord).xy;
      vec4 curr = texture(currentColor, TexCoord);
      vec4 historyColor;
      if (0<lastUV.x && lastUV.x<1 && 0<lastUV.y && lastUV.y<1)
            historyColor = texture(historyTAAResult, lastUV);
      else
            historyColor = curr;
      hdrColor =  curr * (1-weightofHistory) + historyColor * weightofHistory;
      //hdrColor = vec4(abs(texture(motionVector, TexCoord).xy), 0, 1);
}
