#pragma once
#include "core/shape.h"
#include <QOpenGLExtraFunctions>
#include <string>
#include <memory>
#include <assimp/scene.h>           // Output data structure

class Triangle;
class TriangleMesh {
      // for generated meshes, those transforms are copied from the Shape object.
      Transform world2obj, obj2world;
      uint16_t vertex_num = 0;
      uint16_t vbuffer_size = 0;
      void* vertex_data = nullptr;
      uint16_t index_num = 0;
      void* index_data = nullptr;
      GLuint _vao = 0; // vertex array object
      GLuint _vbo = 0; // vertex buffer object
      GLuint _ebo = 0; // element buffer object
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
            bool normalized = false;
            GLint offset;     // offset in the strip
            void* data_ptr;
      };
      using Layout = std::vector<LayoutItem>;
private:
      Layout layout;
      GLenum indexFormat = GL_UNSIGNED_INT; // 4 byte int
public:

      TriangleMesh() = default;
      TriangleMesh(void* raw_data, Layout l, uint16_t vb_size, uint16_t vertex_num, void* index_data, uint16_t index_num, GLenum idxFormat, Transform obj2world)
            : vertex_data(raw_data), layout(l), vbuffer_size(vb_size), vertex_num(vertex_num), 
            index_data(index_data), index_num(index_num), indexFormat(idxFormat) {
            obj2world.Inverse(&world2obj);
            obj2world.setInverse(&world2obj);
            world2obj.setInverse(&obj2world);
      }
      void load(QOpenGLExtraFunctions* f);
      GLuint vao() const { return _vao; }
      GLuint vbo() const { return _vbo; }
      GLuint ebo() const { return _ebo; }
      ~TriangleMesh() { if (vertex_data) delete[] (char*)vertex_data; }
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

std::vector<TriangleMesh*> LoadMeshes(const aiScene* scene);