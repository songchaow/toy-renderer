#pragma once
#include "core/shape.h"
#include <QOpenGLFunctions_4_0_Core>
#include <string>
#include <memory>
#include <assimp/scene.h>           // Output data structure

class Triangle;

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
      GLenum normalized = GL_FALSE;
      GLint offset;     // offset in the strip
      void* data_ptr;
      LayoutItem(uint16_t strip, ArrayType type, GLenum e_format, GLint e_size,
            uint16_t e_count, bool normalized, GLint offset, void* data_ptr) : strip(strip), type(type),
            e_format(e_format), e_size(e_size), e_count(e_count), normalized(normalized), offset(offset), data_ptr(data_ptr) {}
      LayoutItem() = default;
};

class TriangleMesh {
      // for generated meshes, those transforms are copied from the Shape object.
      Transform _world2obj, _obj2world;
      uint32_t vertex_num = 0;
      uint32_t vbuffer_size = 0;
      void* vertex_data = nullptr;
      uint32_t face_num = 0;
      void* index_data = nullptr;
      GLuint _vao = 0; // vertex array object
      GLuint _vbo = 0; // vertex buffer object
      GLuint _ebo = 0; // element buffer object
public:
      using Layout = std::vector<LayoutItem>;
private:
      Layout layout;
      GLenum indexFormat = GL_UNSIGNED_INT; // 4 byte int
public:

      TriangleMesh() = default;
      TriangleMesh(void* raw_data, Layout l, uint32_t vb_size, uint32_t vertex_num, void* index_data, uint16_t index_num, GLenum idxFormat, Transform obj2world)
            : vertex_data(raw_data), layout(l), vbuffer_size(vb_size), vertex_num(vertex_num), 
            index_data(index_data), face_num(index_num), indexFormat(idxFormat) {
            obj2world.Inverse(&_world2obj);
            obj2world.setInverse(&_world2obj);
            _world2obj.setInverse(&obj2world);
      }
      TriangleMesh(const TriangleMesh& t) : _world2obj(t._world2obj), _obj2world(t._obj2world), vertex_num(t.vertex_num),
      vbuffer_size(t.vbuffer_size), face_num(t.face_num), layout(t.layout), indexFormat(t.indexFormat) {
            vertex_data = new char[vbuffer_size];
            std::memcpy(vertex_data, t.vertex_data, vbuffer_size);
            index_data = new char[3 * 4 * face_num];
            std::memcpy(index_data, t.index_data, 3  * face_num);
      }
      void load(QOpenGLFunctions_4_0_Core* f);
      GLuint vao() const { return _vao; }
      GLuint vbo() const { return _vbo; }
      GLuint ebo() const { return _ebo; }
      GLuint face_count() const { return face_num; }
      const Transform& obj2world() const { return _obj2world; }
      const Transform& world2obj() const { return _world2obj; }
      ~TriangleMesh() { if (vertex_data) delete[] (char*)vertex_data; }
};

struct Layout {
      std::vector<LayoutItem> _data;
      Layout() = default;
      Layout(const std::vector<LayoutItem> data) :_data(data) {}
      // TODO
      void emplace_back(const ArrayType& type) {}
      void emplace_back(ArrayType type, GLenum e_format, GLint e_size,
            uint16_t e_count, bool normalized) {
            uint16_t newStrip = 0;
            uint16_t currOffset = 0;
            if (_data.size() == 0)
                  newStrip = e_size * e_count;
            else {
                  newStrip = _data.back().strip + e_size * e_count;
                  currOffset = _data.back().strip;
                  for (auto& i : _data)
                        i.strip = newStrip;
            }
            _data.emplace_back(newStrip, type, e_format, e_size, e_count, normalized,
                  currOffset, nullptr);
      }
      uint16_t strip() const { return _data.front().strip; }
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