# version 330 core
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

uniform mat4 obj2world;
uniform mat4 world2cam;
uniform mat4 cam2ndc;
uniform sampler2D normalSampler; // initially 0-1

in vec2 TexCoord[];
in vec3 posWorld[];
in vec3 normalWorld[];
in vec4 tangentWorld[];

out vec3 pColor;

void GenLine(int index) {
      gl_Position = gl_in[index].gl_Position;
      pColor = vec3(1.0, 0, 0);
      EmitVertex();
      vec3 Nt = vec3(texture(normalSampler, TexCoord[index]));
      Nt = normalize(2*Nt+vec3(-1.0));
      vec3 bitangent = cross(normalWorld[index], tangentWorld[index].xyz);
      vec3 N = normalize( Nt.x * tangentWorld[index].xyz + Nt.y * bitangent + Nt.z * normalWorld[index]);
      gl_Position = cam2ndc * world2cam * vec4(posWorld[index] + N * 0.05, 1.0);
      //gl_Position = gl_in[index].gl_Position + vec4(normalize(tangentWorld[index].xyz), 0)*0.10;
      pColor = vec3(0, 0, 1.0);
      EmitVertex();
      EndPrimitive();
}

void main() {
      GenLine(0);
      GenLine(1);
      GenLine(2);
}