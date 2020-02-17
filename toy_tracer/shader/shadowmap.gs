# version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 18) out;

uniform mat4 world2cams[6];
uniform mat4 cam2ndc;

in vec3 posWorld[];
out vec3 posWorldFs;

void main() {
      for(int face = 0; face < 6; face++) {
            // layer
            gl_Layer = face;
            for(int j = 0; j < 3; j++) {
                  gl_Position =  cam2ndc * world2cams[face] * gl_in[j].gl_Position;
                  posWorldFs = posWorld[j];
                  EmitVertex();
            }
            EndPrimitive();
      }
}