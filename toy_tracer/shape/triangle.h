#pragma once
#include "core/shape.h"
#include <glad/glad.h>
#include <string>
#include <memory>

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
      uint16_t strip;   // same in all items, size of the item in byte
      ArrayType type;
      GLenum e_format;    // type of component in one item
      GLint e_size;     // size of one component in byte
      uint16_t e_count; // component count in one item
      GLenum normalized = GL_FALSE;
      GLint offset;     // offset in the strip
      void* data_ptr;
      bool valid = true;
      LayoutItem(uint16_t strip, ArrayType type, GLenum e_format, GLint e_size,
            uint16_t e_count, bool normalized, GLint offset, void* data_ptr) : strip(strip), type(type),
            e_format(e_format), e_size(e_size), e_count(e_count), normalized(normalized), offset(offset), data_ptr(data_ptr) {}
      LayoutItem() = default;
};

static LayoutItem DEFAULT_POINT3F_LAYOUT = LayoutItem(12, ARRAY_VERTEX, GL_FLOAT, 4, 3, GL_FALSE, 0, nullptr);
static LayoutItem DEFAULT_POINT2F_LAYOUT = LayoutItem(8, ARRAY_VERTEX, GL_FLOAT, 4, 2, GL_FALSE, 0, nullptr);
static LayoutItem DEFAULT_UINT8_LAYOUT = LayoutItem(8, ARRAY_INDEX, GL_UNSIGNED_BYTE, 1, 1, GL_FALSE, 0, nullptr);
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
      Layout(const std::initializer_list<LayoutItem> item_list) : Layout(std::vector<LayoutItem>(item_list)) {}
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
      LayoutItem* Find(ArrayType t) {
            for (auto it = _data.begin(); it < _data.end(); it++)
                  if (it->type == t)
                        return &*it;
            return nullptr;
      }
      const LayoutItem* getLayoutItem(ArrayType t) const;
};

class TriangleMesh {
      // for generated meshes, those transforms are copied from the Shape object.
      Transform _world2obj, _obj2world;
      Layout _layout;
      LayoutItem _indexLayout;
      uint32_t vertex_num = 0;
      // size in byte
      uint32_t vbuffer_size = 0;
      void* vertex_data = nullptr;
      uint32_t face_num = 0;
      char* index_data = nullptr;
      GLuint _primitiveMode = GL_TRIANGLES;
      GLuint _vao = 0; // vertex array object
      GLuint _vbo = 0; // vertex buffer object
      GLuint _ebo = 0; // element buffer object
      //  GLuint _normTexture = 0;  // normal buffer texture
      SphereBound _sb;
public:
      struct CircleBound {
            Point2f center;
            Float size;
      };
private:
      CircleBound _cb;
      AABB _aabb;
      bool glLoaded = false;
private:
      uint8_t indexElementSize = 0;
      GLenum indexFormat = GL_UNSIGNED_INT; // 4 byte int
public:

      TriangleMesh() = default;
      // vertex buffer format: X32Y32Z32
      TriangleMesh(void* vbuffer, Layout l, uint32_t vertex_num, char* index_data, uint32_t faceNum, GLenum idxFormat, Transform obj2world, GLenum primMode)
            : vertex_num(vertex_num), vertex_data(vbuffer), _layout(l), vbuffer_size(vertex_num*l.strip()),
            index_data(index_data), face_num(faceNum), indexFormat(idxFormat), _primitiveMode(primMode) {
            switch (indexFormat) {
            case GL_INT:
            case GL_UNSIGNED_INT:
                  indexElementSize = 4; break;
            case GL_SHORT:
            case GL_UNSIGNED_SHORT:
                  indexElementSize = 2; break;
            case GL_UNSIGNED_BYTE:
            case GL_BYTE:
                  indexElementSize = 1; break;
            }
            obj2world.Inverse(&_world2obj);
            obj2world.setInverse(&_world2obj);
            _world2obj.setInverse(&obj2world);
            // calculate tangent vectors if none
            LayoutItem* tangentL = _layout.Find(ARRAY_TANGENT);
            if (tangentL && tangentL->valid==false) {
                  //fillTangent();
                  tangentL->valid = true;
            }
      }
      TriangleMesh(void* vbuffer, Layout l, uint32_t vertex_num, char* index_data, uint32_t faceNum, GLenum idxFormat, Transform obj2world)
            : TriangleMesh(vbuffer, l, vertex_num, index_data, faceNum, idxFormat, obj2world, GL_TRIANGLES) {}
      TriangleMesh(const TriangleMesh& t) : _world2obj(t._world2obj), _obj2world(t._obj2world), vertex_num(t.vertex_num),
      vbuffer_size(t.vbuffer_size), face_num(t.face_num), _layout(t._layout), indexFormat(t.indexFormat), _primitiveMode(t._primitiveMode) {
            vertex_data = new char[vbuffer_size];
            std::memcpy(vertex_data, t.vertex_data, vbuffer_size);
            index_data = new char[3 * face_num * indexElementSize];
            std::memcpy(index_data, t.index_data, 3 * sizeof(uint32_t) * face_num);
      }
      void load();
      void glUse() { glBindVertexArray(_vao); }
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
      uint8_t sizeofIndexElement() const { return indexElementSize; }
      GLenum indexElementT() const { return indexFormat; }
      GLenum primitiveMode() const { return _primitiveMode; }
      void fillTangent();
      void calcAABB();
      void calcLocalSphereBound();
      void calcXZCircleBound();
      ~TriangleMesh() { 
            if (vertex_data) delete[](char*)vertex_data; 
            if (index_data) delete[](char*)index_data;
            // TODO: unload somewhere if glLoaded is true
      }
      const char* face_triangle(int faceIdx) const { return index_data + faceIdx * 3 * indexElementSize; }
      static TriangleMesh screenMesh;
      const AABB aabb() const { return _aabb; }
      const CircleBound cb() const { return _cb; }
      const SphereBound sb() const { return _sb; }
};

TriangleMesh CreateTriangleMesh(void* vbuffer, Layout l, uint32_t vertex_num, char* index_data, uint32_t faceNum, GLenum idxFormat, Transform obj2world, GLenum primMode);

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

//std::vector<TriangleMesh*> LoadMeshes(const aiScene* scene);
extern std::map<ArrayType, uint16_t> ShaderLocMap;