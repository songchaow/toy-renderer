#pragma once
#include "glad/glad.h"
#include "core/primitive.h"
#include <string>
#include <cctype>
#include <vector>

// buffer
struct glTFBufferFile {
      std::string path;
      uint32_t byteLength;
};
// buffer view
struct glTFBufferView {
      uint16_t bufferIdx;
      uint32_t byteLength;
      uint32_t byteOffset;
};
enum ElementType {
      BYTE = 5120,
      UNSIGNED_BYTE = 5121,
      SHORT = 5122,
      UNSIGNED_SHORT = 5123,
      UNSIGNED_INT = 5125,
      FLOAT = 5126
};
enum UnitType {
      SCALAR,
      VEC2, VEC3, VEC4,
      MAT2, MAT3, MAT4
};
// buffer accessor
struct glTFBufferAccessor {
      uint16_t bufferViewIdx;
      ElementType elementT;
      UnitType unitT; // a unit contains multiple elements
      uint32_t byteOffset;
      uint32_t unitCount;
      // minvalue, maxvalue
};
// sampler
struct glTFSampler {
      GLenum magFilter;
      GLenum minFilter;
      GLenum wrapS;
      GLenum wrapT;
};
// material
struct glTFMaterial {

};

std::vector<Primitive3D*> LoadGLTF(std::string path);

