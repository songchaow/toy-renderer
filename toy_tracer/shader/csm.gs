# version 330 core
# define NUM_CASCADE_MAP 4
layout (triangles) in;
layout (triangle_strip, max_vertices = 12) out;
uniform mat4 world2ndcs[NUM_CASCADE_MAP];

in vec3 posWorld[];
in vec2 TexCoord[];
out vec3 posWorldFs;
out float depth;
out vec2 texCoord;

void main() {
      for(int face = 0; face < NUM_CASCADE_MAP; face++) {
            // layer
            gl_Layer = face;
            for(int j = 0; j < 3; j++) {
                  gl_Position =  world2ndcs[face] * gl_in[j].gl_Position;
                  posWorldFs = posWorld[j];
                  depth = gl_Position.z / gl_Position.w;
                  texCoord = TexCoord[j];
                  EmitVertex();
            }
            EndPrimitive();
      }
}