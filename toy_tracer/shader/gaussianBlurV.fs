#version 330 core
in vec2 TexCoord;
out vec4 blurColor;
uniform sampler2D color;
#define weight0 0.199676
#define weight1 0.176213
#define weight2 0.121109
#define weight3 0.064825
#define weight4 0.027023
#define weight5 0.008773
#define weight6 0.002218
void main() {
      vec4 sum = vec4(0.0);
      float pixelOffset = 1.0 / 800.0;
      sum += texture(color, TexCoord) * weight0;
      sum += texture(color, vec2(TexCoord.x, TexCoord.y-1*pixelOffset)) * weight1;
      sum += texture(color, vec2(TexCoord.x, TexCoord.y-2*pixelOffset)) * weight2;
      sum += texture(color, vec2(TexCoord.x, TexCoord.y-3*pixelOffset)) * weight3;
      sum += texture(color, vec2(TexCoord.x, TexCoord.y-4*pixelOffset)) * weight4;
      sum += texture(color, vec2(TexCoord.x, TexCoord.y-5*pixelOffset)) * weight5;
      sum += texture(color, vec2(TexCoord.x, TexCoord.y-6*pixelOffset)) * weight6;

      sum += texture(color, vec2(TexCoord.x, TexCoord.y+1*pixelOffset)) * weight1;
      sum += texture(color, vec2(TexCoord.x, TexCoord.y+2*pixelOffset)) * weight2;
      sum += texture(color, vec2(TexCoord.x, TexCoord.y+3*pixelOffset)) * weight3;
      sum += texture(color, vec2(TexCoord.x, TexCoord.y+4*pixelOffset)) * weight4;
      sum += texture(color, vec2(TexCoord.x, TexCoord.y+5*pixelOffset)) * weight5;
      sum += texture(color, vec2(TexCoord.x, TexCoord.y+6*pixelOffset)) * weight6;
      blurColor = sum;
}