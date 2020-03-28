#include "shape/triangle.h"
#include <cstring>
#include <map>

#if 0
void addMesh(const aiNode* node, const aiScene* scene, aiMatrix4x4 local2world, std::vector<TriangleMesh*>& meshes) {
      aiMatrix4x4 o2w = local2world * node->mTransformation;
      for (int idx = 0; idx < node->mNumMeshes; idx++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[idx]];
            unsigned int vertex_count = mesh->mNumVertices;
            Layout layout;
            uint16_t curr_strip = 0;
            // points
            {
                  LayoutItem meshLayout;
                  meshLayout.type = ArrayType::ARRAY_VERTEX;
                  meshLayout.e_count = 3;
                  meshLayout.e_format = GL_FLOAT;
                  meshLayout.e_size = sizeof(ai_real);
                  meshLayout.offset = curr_strip;
                  meshLayout.data_ptr = mesh->mVertices;
                  layout.push_back(meshLayout);
                  curr_strip += meshLayout.e_size*meshLayout.e_count;
            }
            // check if have normals
            if (mesh->HasNormals()) {
                  LayoutItem normLayout = {};
                  normLayout.type = ArrayType::ARRAY_NORMAL;
                  normLayout.e_count = 3;
                  // ai_real(float) same size as GL_FLOAT?
                  normLayout.e_format = GL_FLOAT;
                  normLayout.e_size = sizeof(ai_real);
                  normLayout.offset = curr_strip;
                  normLayout.data_ptr = mesh->mNormals;
                  layout.push_back(normLayout);
                  curr_strip += normLayout.e_size*normLayout.e_count;
            }
            // check if have texture coordinates
            for (int i = 0; i < 2; i++)
                  if (mesh->HasTextureCoords(i)) {
                        LayoutItem texcoordLayout;
                        texcoordLayout.type = ArrayType::ARRAY_TEX_UV;
                        texcoordLayout.e_count = 2;
                        texcoordLayout.e_format = GL_FLOAT;
                        texcoordLayout.e_size = sizeof(ai_real);
                        texcoordLayout.offset = curr_strip;
                        texcoordLayout.data_ptr = mesh->mTextureCoords[i];
                        layout.push_back(texcoordLayout);
                        curr_strip += texcoordLayout.e_size*texcoordLayout.e_count;
                  }

            // at last, the strip is decided and we fill in data.
            void* raw_data = new char[curr_strip*vertex_count];
            for (int i = 0; i < layout.size(); i++) {
                  auto& item = layout[i];
                  item.strip = curr_strip;
                  char* p_char = (char*)raw_data + item.offset;
                  char* p_src = (char*)item.data_ptr;
                  for (int i = 0; i < vertex_count; i++) {
                        std::memcpy(p_char, p_src, item.e_size*item.e_count);
                        p_char += item.strip;
                        p_src += item.e_size*item.e_count;
                  }
            }
            // copy faces
            uint32_t* idx_data = new uint32_t[3 * mesh->mNumFaces];
            for (int i = 0; i < mesh->mNumFaces; i++) {
                  for (int j = 0; j < 3; j++)
                        idx_data[i * 3 + j] = mesh->mFaces[i].mIndices[j];
            }
            Transform obj2world(o2w[0][0], o2w[0][1], o2w[0][2], o2w[0][3], o2w[1][0], o2w[1][1], o2w[1][2], o2w[1][3],
                  o2w[2][0], o2w[2][1], o2w[2][2], o2w[2][3], o2w[3][0], o2w[3][1], o2w[3][2], o2w[3][3]);
            TriangleMesh* tri_mesh = new TriangleMesh(raw_data, layout, vertex_count,
                  (char*)idx_data, mesh->mNumFaces, GL_UNSIGNED_INT, obj2world);
            meshes.push_back(tri_mesh);
      }
      for (int i = 0; i < node->mNumChildren; i++) {
            addMesh(node->mChildren[i], scene, o2w, meshes);
      }
}
#endif

#if 0
std::vector<TriangleMesh*> LoadMeshes(const aiScene* scene) {
      if (scene->mNumMeshes == 0)
            return {};
      std::vector<TriangleMesh*> meshes;
      addMesh(scene->mRootNode, scene, aiMatrix4x4(), meshes);
      return meshes;
}
#endif

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
      glLoaded = true;
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

void TriangleMesh::calcLocalSphereBound() {
      unsigned char* pVertexData = static_cast<unsigned char*>(vertex_data);
      const LayoutItem* vertexPosLayout = _layout.getLayoutItem(ArrayType::ARRAY_VERTEX);
      uint16_t vertexPosOffset = vertexPosLayout->offset;

      _aabb = AABB();

      for(int i = 0; i < vertex_num; i++) {
            Point3f* pos = reinterpret_cast<Point3f*>(pVertexData + _layout.strip() * i + vertexPosOffset);
            _aabb.Add(*pos);
      }
      Vector3f halfExtent = _aabb.extent();
      _sb.center = _aabb.center();
      Float radius2 = 0;
      for(int i = 0; i < vertex_num; i++) {
            Point3f* pos = reinterpret_cast<Point3f*>(pVertexData + _layout.strip() * i + vertexPosOffset);
            Float currRadius2 = (*pos - _sb.center).LengthSquared();
            if(currRadius2 > radius2)
                  radius2 = currRadius2;
      }
      _sb.radius = std::sqrt(radius2);
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