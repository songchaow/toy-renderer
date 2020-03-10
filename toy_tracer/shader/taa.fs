#version 330 core

layout (location = 0) out vec4 hdrColor;

in vec2 TexCoord;

uniform sampler2D currentColor;
uniform sampler2D historyTAAResult;
uniform sampler2D motionVector;
uniform float weightofHistory = 0.95;
uniform vec2 windowSize;
#define NUM_PATT_POINT 9
uniform vec2 threebythreePattern[NUM_PATT_POINT] = vec2[](
      vec2(-1, 1),
      vec2(0, 1),
      vec2(1, 1),
      vec2(-1, 0),
      vec2(0, 0),
      vec2(1, 0),
      vec2(-1, -1),
      vec2(-1, 0),
      vec2(-1, 1)
);

void main() {
      float xunit = 1.0 / windowSize.x;
      float yunit = 1.0 / windowSize.y;
      vec4 minColor = texture(currentColor, vec2(TexCoord.x-xunit, TexCoord.y+yunit));
      vec4 maxColor = minColor;
      vec2 maxSpeed = vec2(0);
      // sample 3x3 in current color buffer
      for(int i=1;i<NUM_PATT_POINT;i++) {
            vec2 p = TexCoord + vec2(xunit, yunit) * threebythreePattern[i];
            minColor = min(texture(currentColor, p), minColor);
            maxColor = max(texture(currentColor, p), maxColor);
            vec2 thisSpeed = texture(motionVector, p).rg;
            if(dot(thisSpeed, thisSpeed) > dot(maxSpeed, maxSpeed))
                  maxSpeed = thisSpeed;
      }
      vec2 lastUV = TexCoord + maxSpeed;
      vec4 curr = texture(currentColor, TexCoord);
      vec4 historyColor;
      if (0<lastUV.x && lastUV.x<1 && 0<lastUV.y && lastUV.y<1) {
            // clamp history color
                  // vec4 nb00 = texture(historyTAAResult, vec2(lastUV.x-xunit, lastUV.y+yunit));
            // vec4 nb01 = texture(historyTAAResult, vec2(lastUV.x, lastUV.y+yunit));
            // vec4 nb02 = texture(historyTAAResult, vec2(lastUV.x+xunit, lastUV.y+yunit));
            // vec4 nb10 = texture(historyTAAResult, vec2(lastUV.x-xunit, lastUV.y));
            // vec4 nb12 = texture(historyTAAResult, vec2(lastUV.x+xunit, lastUV.y));;
            // vec4 nb20 = texture(historyTAAResult, vec2(lastUV.x-xunit, lastUV.y-yunit));
            // vec4 nb21 = texture(historyTAAResult, vec2(lastUV.x, lastUV.y-yunit));
            // vec4 nb22 = texture(historyTAAResult, vec2(lastUV.x+xunit, lastUV.y-yunit));
            // vec4 min_nb0 = min(nb00, nb01);
            // vec4 min_nb1 = min(nb02, nb10);
            // vec4 min_nb2 = min(nb12, nb20);
            // vec4 min_nb3 = min(nb21, nb22);
            // vec4 minmin0 = min(min_nb0, min_nb1);
            // vec4 minmin1 = min(min_nb2, min_nb3);
            // vec4 minColor = min(minmin0, minmin1);
            // vec4 max_nb0 = max(nb00, nb01);
            // vec4 max_nb1 = max(nb02, nb10);
            // vec4 max_nb2 = max(nb12, nb20);
            // vec4 max_nb3 = max(nb21, nb22);
            // vec4 maxmax0 = max(max_nb0, max_nb1);
            // vec4 maxmax1 = max(max_nb2, max_nb3);
            // vec4 maxColor = max(maxmax0, maxmax1);
            historyColor = texture(historyTAAResult, lastUV);
            historyColor = clamp(historyColor, minColor, maxColor);
      }
      else
            historyColor = curr;
      hdrColor =  curr * (1-weightofHistory) + historyColor * weightofHistory;
      hdrColor.a = 1.0;
      //hdrColor = vec4(abs(texture(motionVector, TexCoord).xy), 0, 1);
}
