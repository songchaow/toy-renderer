#pragma once
#include "core/shape.h"
#include <glad/glad.h>
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
      GLenum e_format;    // type of element in one item
      GLint e_size;     // size of one element
      uint16_t e_count; // element count in one item
      GLenum normalized = GL_FALSE;
      GLint offset;     // offset in the strip
      void* data_ptr;
      LayoutItem(uint16_t strip, ArrayType type, GLenum e_format, GLint e_size,
            uint16_t e_count, bool normalized, GLint offset, void* data_ptr) : strip(strip), type(type),
            e_format(e_format), e_size(e_size), e_count(e_count), normalized(normalized), offset(offset), data_ptr(data_ptr) {}
      LayoutItem() = default;
};

static LayoutItem DEFAULT_VERTEX_LAYOUT = LayoutItem(12, ARRAY_VERTEX, GL_FLOAT, 4, 3, GL_FALSE, 0, nullptr);
static LayoutItem DEFAULT_TEXUV_LAYOUT = LayoutItem(8, ARRAY_TEX_UV, GL_FLOAT, 4, 2, GL_FALSE, 0, nullptr);
static LayoutItem DEFAULT_NORMAL_LAYOUT = LayoutItem(12, ARRAY_NORMAL, GL_FLOAT, 4, 3, GL_FALSE, 0, nullptr);

struct Layout {
      std::vector<LayoutItem> _data;
      Layout() = default;
      Layout(const std::vector<LayoutItem> data) :_data(data) {
            // fix strip and offset
            uint16_t curr_offset = 0;
            for(auto i = _data.begin();i<_data.end();i++) {
                  i->offset = curr_offset;
                  curr_offset += i->e_size * i->e_count;
            }
            for(auto i = _data.begin();i<_data.end();i++)
                  i->strip = curr_offset;
      }
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
      void push_back(const LayoutItem& l) {
            uint16_t newStrip = 0;
            uint16_t currOffset = 0;
            if (_data.size() == 0)
                  newStrip = l.e_size * l.e_count;
            else {
                  newStrip = _data.back().strip + l.e_size * l.e_count;
                  currOffset = _data.back().strip;
                  for (auto& i : _data)
                        i.strip = newStrip;
            }
            _data.push_back(l);
      }
      uint16_t strip() const { return _data.front().strip; }
      size_t size() const { return _data.size(); }
      LayoutItem& operator[](unsigned int idx) { return _data[idx]; }
      const LayoutItem* getLayoutItem(ArrayType t) const;
};

class TriangleMesh {
      // for generated meshes, those transforms are copied from the Shape object.
      Transform _world2obj, _obj2world;
      uint32_t vertex_num = 0;
      // size in byte
      uint32_t vbuffer_size = 0;
      void* vertex_data = nullptr;
      uint32_t face_num = 0;
      uint32_t* index_data = nullptr;
      GLuint _vao = 0; // vertex array object
      GLuint _vbo = 0; // vertex buffer object
      GLuint _ebo = 0; // element buffer object
      //  GLuint _normTexture = 0;  // normal buffer texture
public:
      //using Layout = std::vector<LayoutItem>;
private:
      Layout _layout;
      GLenum indexFormat = GL_UNSIGNED_INT; // 4 byte int
public:

      TriangleMesh() = default;
      TriangleMesh(void* vbuffer, Layout l, uint32_t vb_size, uint32_t* index_data, uint32_t faceNum, GLenum idxFormat, Transform obj2world)
            : vertex_data(vbuffer), _layout(l), vbuffer_size(vb_size), 
            index_data(index_data), face_num(faceNum), indexFormat(idxFormat) {
            vertex_num = vb_size / _layout.strip();
            obj2world.Inverse(&_world2obj);
            obj2world.setInverse(&_world2obj);
            _world2obj.setInverse(&obj2world);
      }
      TriangleMesh(const TriangleMesh& t) : _world2obj(t._world2obj), _obj2world(t._obj2world), vertex_num(t.vertex_num),
      vbuffer_size(t.vbuffer_size), face_num(t.face_num), _layout(t._layout), indexFormat(t.indexFormat) {
            vertex_data = new char[vbuffer_size];
            std::memcpy(vertex_data, t.vertex_data, vbuffer_size);
            index_data = new uint32_t[3 * face_num];
            std::memcpy(index_data, t.index_data, 3 * sizeof(uint32_t) * face_num);
      }
      void load();
      GLuint vao() const { return _vao; }
      GLuint vbo() const { return _vbo; }
      GLuint ebo() const { return _ebo; }
      //GLuint normalTexture() const { return _normTexture; }
      GLuint face_count() const { return face_num; }
      GLuint vertex_count() const { return vertex_num; }
      const void* vertexData(uint32_t idx) const { return static_cast<char*>(vertex_data) + _layout.strip() * idx; }
      const Transform& obj2world() const { return _obj2world; }
      const Transform& world2obj() const { return _world2obj; }
      const Layout& layout() const { return _layout; }
      ~TriangleMesh() { 
            if (vertex_data) delete[](char*)vertex_data; 
            if (index_data) delete[](char*)index_data;
      }
      const uint32_t* face_triangle(int faceIdx) const { return index_data + faceIdx * 3; }
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