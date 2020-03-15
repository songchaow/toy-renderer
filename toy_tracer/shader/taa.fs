#version 330 core

layout (location = 0) out vec4 hdrColor;
layout (location = 1) out vec3 bloomColor;

in vec2 TexCoord;

uniform sampler2D currentColor;
uniform sampler2D historyTAAResult;
uniform sampler2D motionVector;
uniform float weightofHistory = 0.95;
uniform vec2 windowSize;
#define NUM_PATT_POINT 9
#define bloomThreashold 1.0
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
uniform bool disableClampWhenStatic = true;
uniform mat3 rgb2ycbcr = mat3(0.2126, -0.114572, 0.5,
                              0.7152, -0.385428, -0.454153,
                              0.0722, 0.5,      -0.0458471);
uniform mat3 ycbcr2rgb = mat3(1, 1, 1,
                              0, -0.187324, 1.8556,
                              1.5748, -0.468124, 0);

// clip in YCbCr space
vec3 clipColor(vec3 minColor, vec3 maxColor, vec3 historyColor) {
      // vec3 center = 0.5 * (minColor + maxColor);
      // vec3 halfSizeAbs = maxColor - center;
      // // in cw order
      // vec3 cp = historyColor - center;
      // //cp = clamp(cp, -halfSizeAbs, halfSizeAbs);
      // float xShrink;
      // float yShrink;
      // float zShrink;
      // if(cp.x==0)
      //       xShrink = 1.0;
      // else
      //       xShrink = clamp(abs(halfSizeAbs.x/cp.x), 0, 1);
      // if(cp.y==0)
      //       yShrink = 1.0;
      // else
      //       yShrink = clamp(abs(halfSizeAbs.y/cp.y), 0, 1);
      // if(cp.z==0)
      //       zShrink = 1.0;
      // else
      //       zShrink = clamp(abs(halfSizeAbs.z/cp.z), 0, 1);
      // float minShrink = min(min(xShrink, yShrink), zShrink);
      // cp *= minShrink;
      // return center+cp;
      return clamp(historyColor, minColor, maxColor);
}

void main() {
      float xunit = 1.0 / windowSize.x;
      float yunit = 1.0 / windowSize.y;
      vec3 minColor = texture(currentColor, vec2(TexCoord.x-xunit, TexCoord.y+yunit)).rgb;
      vec3 maxColor = minColor;
      minColor = rgb2ycbcr * minColor;
      maxColor = rgb2ycbcr * maxColor;
      vec2 maxSpeed = texture(motionVector, vec2(TexCoord.x-xunit, TexCoord.y+yunit)).rg;
      // sample 3x3 in current color buffer
      for(int i=1;i<NUM_PATT_POINT;i++) {
            vec2 p = TexCoord + vec2(xunit, yunit) * threebythreePattern[i];
            vec3 currColorRGB = texture(currentColor, p).rgb;
            vec3 currColorYUV = rgb2ycbcr * currColorRGB;
            minColor = min(currColorYUV, minColor);
            maxColor = max(currColorYUV, maxColor);
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
            if(historyColor.a < 500)
                  historyColor.a = historyColor.a + 1.0;
            bool moving = any(notEqual(maxSpeed, vec2(0)));
            if(moving || (!moving && !disableClampWhenStatic)) {
                  // do clamp check
                  vec3 historyColorYUV = rgb2ycbcr * historyColor.rgb;
                  vec3 historyColorYUVClamped = clipColor(minColor, maxColor, historyColorYUV);
                  vec3 diff = abs(historyColorYUV - historyColorYUVClamped);
                  historyColor.rgb = ycbcr2rgb * historyColorYUVClamped;
                  if(any(greaterThan(diff, vec3(0.05))))
                        // a clamp has happened
                        historyColor.a = 0.0;
            }
      }
      else
            // for new pixels, assume clamping haven't happened to them.
            historyColor = vec4(curr.rgb, 500);
      // weightofHistoryD increases if history is near clamping, i.e., history.a is small
      float t = max(0, 1 - historyColor.a / 500);
      float weightofHistoryD = mix(0.95, 0.999, t);
      //weightofHistoryD = 0.95;
      hdrColor =  curr * (1-weightofHistoryD) + historyColor * weightofHistoryD; // next history color
      hdrColor.a = historyColor.a;
      //hdrColor = vec4(abs(texture(motionVector, TexCoord).xy), 0, 1);
      // bloom
      float brightness =  dot(hdrColor.rgb, vec3(0.2126, 0.7152, 0.0722));
      if(brightness > bloomThreashold) {
            bloomColor = hdrColor.rgb;
            bloomColor = vec3(0);
      }
}
