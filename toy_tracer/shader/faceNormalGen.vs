#version 330 core
layout (location = 0) in uvec3 faceIdx;
in int gl_VertexID; //debug
uniform samplerBuffer posSampler;
out vec3 faceNormal;

void main() {
      vec3 p1 = texelFetch(posSampler, int(faceIdx.x)).xyz;
      vec3 p2 = texelFetch(posSampler, int(faceIdx.y)).xyz;
      vec3 p3 = texelFetch(posSampler, int(faceIdx.z)).xyz;
      vec3 v1 = p2 - p1;
      vec3 v2 = p3 - p1;
      gl_Position = vec4(0.5, 0.5, 1.0, 1.0);
      faceNormal = normalize(cross(v2, v1));
//      float newX = float(faceIdx.x);
//      newX = newX / 2452;
//      float newY = float(faceIdx.y);
//      newY = newY / 2452;
//      float newZ = float(faceIdx.z);
//      newZ = newZ / 2452;
//      faceNormal = vec3(newX, newY, newZ);
     //faceNormal = vec3(gl_VertexID/5000.f, 0, 0);
}