#version 330 core
layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

in VertexOutput {
    vec2 sizeNDC;
} VOutput[];

out GeometryOutput {
    vec2 TexCoord;
} GOutput;

void main() {
    gl_Position = gl_in[0].gl_Position;
    GOutput.TexCoord = vec2(0,0);
    EmitVertex();
    gl_Position = gl_in[0].gl_Position + vec4(VOutput[0].sizeNDC.x,0,0,0);
    GOutput.TexCoord = vec2(1,0);
    EmitVertex();
    gl_Position = gl_in[0].gl_Position + vec4(0,VOutput[0].sizeNDC.y,0,0);
    GOutput.TexCoord = vec2(0,1);
    EmitVertex();
    gl_Position = gl_in[0].gl_Position + vec4(VOutput[0].sizeNDC,0,0);
    GOutput.TexCoord = vec2(1,1);
    EmitVertex();
    EndPrimitive();
}