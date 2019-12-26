#version 330 core
in uvec3 faceIdx;
uniform sampler1D posSampler;
out vec3 normal;

void main() {
      vec3 p1 = vec3(texelFetch(posSampler, faceIdx.x));
      vec3 p2 = vec3(texelFetch(posSampler, faceIdx.y));
      vec3 p3 = vec3(texelFetch(posSampler, faceIdx.z));
      vec3 v1 = p2 - p1;
      vec3 v2 = p3 - p1;
      normal = normalize(cross(v1, v2));
}