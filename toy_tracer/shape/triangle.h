#pragma once
#include "core/shape.h"
#include <QOpenGLExtraFunctions>
#include <string>
#include <memory>

class Triangle;
class TriangleMesh {
      // for generated meshes, those transforms are copied from the Shape object.
      Transform world2obj, obj2world;
      void* raw_data = nullptr;
      std::vector<Triangle> face_indices;
public:
      enum ArrayType {
            ARRAY_VERTEX = 0,
            ARRAY_NORMAL = 1,
            ARRAY_TANGENT = 2,
            ARRAY_COLOR = 3,
            ARRAY_TEX_UV = 4,
            ARRAY_TEX_UV2 = 5,
            ARRAY_BONES = 6,
            ARRAY_WEIGHTS = 7,
            ARRAY_INDEX = 8,
            ARRAY_MAX = 9
      };
      struct LayoutItem {
            uint16_t strip;   // same in all items
            ArrayType type;
            GLenum e_format;    // element type in one unit
            GLint e_size;     // element size(in byte)
            uint16_t e_count; // element count in one unit
            GLint offset;     // offset in the strip
            void* data_ptr;
      };
      using Layout = std::vector<LayoutItem>;
private:
      Layout layout;
public:

      TriangleMesh() = default;
      TriangleMesh(void* raw_data, Layout l) : raw_data(raw_data), layout(l) {}
      ~TriangleMesh() { if (raw_data) delete[] (char*)raw_data; }
};

class Triangle : public Shape {
      TriangleMesh* mesh;
      //
      uint32_t index0, index1, index2;
public:
      virtual Float Area() const override;
      // const GLenum GLIndexType() const { return GL_UNSINGED_INT; }
      // ctor: create a triangle in one new TriangleMesh, with Point3f as input
      // ctor: create a triangle using existing TriangleMesh, with Point3f as input
      // ctor: create a triangle using existing TriangleMesh

};

TriangleMesh* LoadTriangleMesh(const std::string& path);