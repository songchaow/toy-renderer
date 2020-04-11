#include "shape/triangle.h"
#include <cstring>
#include <map>



std::map<ArrayType, uint16_t> ShaderLocMap = {
      {ArrayType::ARRAY_VERTEX, 0},
      {ArrayType::ARRAY_TEX_UV, 1},
      {ArrayType::ARRAY_NORMAL, 2},
      {ArrayType::ARRAY_TANGENT, 3}
};

void TriangleMesh::load() {
      // GLuint norms = GenFaceNormal_GPU(*this, f);
      // // normal texture
      // glGenTextures(1, &_normTexture);
      // glBindTexture(GL_TEXTURE_BUFFER, _normTexture);
      // //glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, tmp_buff);
      // glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, norms);
      if (_vao > 0)
            return;
      glGenVertexArrays(1, &_vao);
      glBindVertexArray(_vao);
      // vbo
      glGenBuffers(1, &_vbo);
      glBindBuffer(GL_ARRAY_BUFFER, _vbo);
      glBufferData(GL_ARRAY_BUFFER, vbuffer_size, vertex_data, GL_STATIC_DRAW);
      // ebo
      glGenBuffers(1, &_ebo);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
      uint32_t res = indexElementSize * 3 * face_num;
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexElementSize * 3 * face_num, index_data, GL_STATIC_DRAW);
      // configure vertex pointers (stored in vao)
      for (int i = 0; i < _layout.size(); i++) {
            auto& l = _layout[i];
            if (ShaderLocMap.find(l.type) == ShaderLocMap.end())
                  // not yet configured in vertex shader
                  continue;
            uint16_t shaderLoc = ShaderLocMap[l.type];
            glVertexAttribPointer(shaderLoc, l.e_count, l.e_format, l.normalized, l.strip, (void*)l.offset);
            glEnableVertexAttribArray(shaderLoc);
      }

      glBindVertexArray(0);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void TriangleMesh::fillTangent()
{
      LayoutItem* posLayout = _layout.Find(ARRAY_VERTEX);
      LayoutItem* texuvLayout = _layout.Find(ARRAY_TEX_UV);
      LayoutItem* tangentLayout = _layout.Find(ARRAY_TANGENT);
      uint32_t layoutStrip = _layout.strip();
      if (!texuvLayout || !posLayout || !tangentLayout)
            return;
      //bool* calculated = new bool[vertex_num]();
      std::vector<uint16_t> facePerVertex(vertex_num, 0);
      for (int i = 0; i < face_num; i++) {
            // iterate through faces
            unsigned char* pFace = (unsigned char*)index_data + 3 * i * indexElementSize;
            uint32_t faceIdx[3];
            if (indexElementSize == 1) {
                  for (int j = 0; j < 3; j++) {
                        faceIdx[j] = *(pFace + j);
                  }
            }
            else if (indexElementSize == 2) {
                  for (int j = 0; j < 3; j++) {
                        uint16_t* pShort = reinterpret_cast<uint16_t*>(pFace);
                        faceIdx[j] = pShort[j];
                  }
            }
            else if (indexElementSize == 4) {
                  for (int j = 0; j < 3; j++) {
                        uint32_t* pInt = reinterpret_cast<uint32_t*>(pFace);
                        faceIdx[j] = pInt[j];
                  }
            }
            // we assume UVs, tangents and positions are both stored in 32-bit floats
            Point2f* texUV[3];
            Point3f* positions[3];
            Vector3f* tangents[3];
            for (int j = 0; j < 3; j++) {
                  const uint32_t& currVerIdx = faceIdx[j];
                  texUV[j] = reinterpret_cast<Point2f*>((char*)vertex_data + texuvLayout->offset + layoutStrip*currVerIdx);
                  positions[j] = reinterpret_cast<Point3f*>((char*)vertex_data + posLayout->offset + layoutStrip * currVerIdx);
                  tangents[j] = reinterpret_cast<Vector3f*>((char*)vertex_data + tangentLayout->offset + layoutStrip * currVerIdx);
            }
            for (int j = 0; j < 3; j++) {
                  const uint32_t& currVerIdx = faceIdx[j];
                  int a = (j + 1) % 3;
                  int b = (j + 2) % 3;
                  Vector3f dir1 = *positions[a] - *positions[j];
                  Vector3f dir2 = *positions[b] - *positions[j];
                  Vector2f deltauv1 = *texUV[a] - *texUV[j];
                  Vector2f deltauv2 = *texUV[b] - *texUV[j];
                  uint16_t& numFaces = facePerVertex[currVerIdx];
                  if (numFaces == 0)
                        *tangents[j] = Normalize(deltauv2.y * dir1 - deltauv1.y * dir2);
                  else
                        *tangents[j] += Normalize(deltauv2.y * dir1 - deltauv1.y * dir2);
                  numFaces++;
                  //Vector3f dirperV = 
            }
            
      }
      // TODO: normalize all tangents
      for (int i = 0; i < vertex_num; i++) {
            uint16_t numFaces = facePerVertex[i];
            if (numFaces == 0) {
                  LOG(WARNING) << "unreferenced vertex";
                  continue;
            }
            Vector3f* tangent = reinterpret_cast<Vector3f*>((char*)vertex_data + tangentLayout->offset + layoutStrip * i);
            *tangent /= numFaces;
            // handness
            ((Float*)tangent)[3] = 1.0;
      }
}

const LayoutItem* Layout::getLayoutItem(ArrayType t) const {
      for (auto& l : _data) {
            if (l.type == t)
                  return &l;
      }
}

static Float screenVertex[] = {
      // Position       Tex coord
      -1.f, -1.f,       0.f, 0.f,
      -1.f, 1.f,        0.f, 1.f,
      1.f, -1.f,        1.f, 0.f,
      1.f, 1.f,         1.f, 1.f
};
static uint32_t screenMeshIndices[] = {
      1, 0, 2, 1, 2, 3
};

TriangleMesh TriangleMesh::screenMesh = TriangleMesh(screenVertex, std::vector<LayoutItem>{DEFAULT_POINT2F_LAYOUT, DEFAULT_TEXUV_LAYOUT},
 4, (char*)screenMeshIndices, 2, GL_UNSIGNED_INT, Transform::Identity());