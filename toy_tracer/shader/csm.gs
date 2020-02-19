# version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 18) out;
# define NUM_CASCADE_MAP 4
uniform mat4 world2ndcs[NUM_CASCADE_MAP];

in vec3 posWorld[];
out vec3 posWorldFs;

void main() {
      for(int face = 0; face < NUM_CASCADE_MAP; face++) {
            // layer
            gl_Layer = face;
            for(int j = 0; j < 3; j++) {
                  gl_Position =  world2ndcs[face] * gl_in[j].gl_Position;
                  posWorldFs = posWorld[j];
                  EmitVertex();
            }
            EndPrimitive();
      }
}