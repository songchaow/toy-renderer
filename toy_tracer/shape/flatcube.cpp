#include "shape/flatcube.h"
#include "shape/triangle.h"

static Layout _flatcubeLayout = { DEFAULT_POINT3F_LAYOUT, DEFAULT_TEXUV_LAYOUT, DEFAULT_NORMAL_LAYOUT };

static Float _flatcubeVertexData[] = {
      // pos                  // tex            // normal
      // UP y fixed
      0.5, 0.5, 0.5,          1.0, 1.0,         0.0, 1.0, 0.0,
      0.5, 0.5, -0.5,         1.0, 0.0,         0.0, 1.0, 0.0,
      -0.5, 0.5, 0.5,         0.0, 1.0,         0.0, 1.0, 0.0,
      -0.5, 0.5, -0.5,         0.0, 0.0,         0.0, 1.0, 0.0,
      // DOWN
      0.5, -0.5, 0.5,         1.0, 1.0,         0.0, -1.0, 0.0,
      0.5, -0.5, -0.5,        1.0, 0.0,         0.0, -1.0, 0.0,
      -0.5, -0.5, 0.5,        0.0, 1.0,         0.0, -1.0, 0.0,
      -0.5, -0.5, -0.5,       0.0, 0.0,         0.0, -1.0, 0.0,
      // LEFT x fixed
      -0.5, 0.5, 0.5,         1.0, 1.0,         -1.0, 0.0, 0.0,
      -0.5, 0.5, -0.5,        1.0, 0.0,         -1.0, 0.0, 0.0,
      -0.5, -0.5, 0.5,        0.0, 1.0,         -1.0, 0.0, 0.0,
      -0.5, -0.5, -0.5,       0.0, 0.0,         -1.0, 0.0, 0.0,
      // RIGHT
      0.5, 0.5, 0.5,          1.0, 1.0,         1.0, 0.0, 0.0,
      0.5, 0.5, -0.5,         1.0, 0.0,         1.0, 0.0, 0.0,
      0.5, -0.5, 0.5,         0.0, 1.0,         1.0, 0.0, 0.0,
      0.5, -0.5, -0.5,        0.0, 0.0,         1.0, 0.0, 0.0,
      // FRONT z fixed
      0.5, 0.5, -0.5,         1.0, 1.0,         0.0, 0.0, -1.0,
      0.5, -0.5, -0.5,        1.0, 0.0,         0.0, 0.0, -1.0,
      -0.5, 0.5, -0.5,        0.0, 1.0,         0.0, 0.0, -1.0,
      -0.5, -0.5, -0.5,       0.0, 0.0,         0.0, 0.0, -1.0,
      // BACK 
      0.5, 0.5, 0.5,          1.0, 1.0,         0.0, 0.0, 1.0,
      0.5, -0.5, 0.5,         1.0, 0.0,         0.0, 0.0, 1.0,
      -0.5, 0.5, 0.5,         0.0, 1.0,         0.0, 0.0, 1.0,
      -0.5, -0.5, 0.5,        0.0, 0.0,         0.0, 0.0, 1.0,
};

uint16_t FlatCube::_flatCubeIndices[] = {
      0, 1, 2,
      1, 3, 2,

      4, 6, 5,
      5, 6, 7,

      8, 9, 10,
      10, 9, 11,

      12, 14, 13,
      13, 14, 15,

      16, 17, 18,
      17, 19, 18,

      20, 22, 21,
      21, 22, 23,
};

std::vector<TriangleMesh*> FlatCube::GenMesh() const
{
      constexpr uint32_t numVertice = 4 * 6;
      constexpr uint32_t faceNum = 2 * 6;
      sizeof(_flatcubeVertexData);
      char* newVertexData = new char[sizeof(_flatcubeVertexData)];
      std::memcpy(newVertexData, _flatcubeVertexData, sizeof(_flatcubeVertexData));
      // scale the vertex position
      int strip = _flatcubeLayout.strip();
      for (unsigned int i = 0; i < numVertice; i++) {
            Float* pos = reinterpret_cast<Float*>(newVertexData + i * strip);
            *pos *= cubeLength;
            *(pos + 1) *= cubeLength;
            *(pos + 2) *= cubeLength;
      }
      TriangleMesh* m = new TriangleMesh(newVertexData, _flatcubeLayout, numVertice, (char*)_flatCubeIndices, faceNum, GL_UNSIGNED_SHORT, Transform::Identity()); ;
      return std::vector<TriangleMesh*>(1, m);
}
