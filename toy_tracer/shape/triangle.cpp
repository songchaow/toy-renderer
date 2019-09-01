#include "shape/triangle.h"
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include <assimp/vector3.h>
#include <cstring>
#include <map>

void addMesh(const aiNode* node, const aiScene* scene, aiMatrix4x4 local2world, std::vector<TriangleMesh*>& meshes) {
      aiMatrix4x4 o2w = local2world * node->mTransformation;
      for (int idx = 0; idx < node->mNumMeshes; idx++) {
            aiMesh* mesh = scene->mMeshes[scene->mRootNode->mMeshes[idx]];
            unsigned int vertex_count = mesh->mNumVertices;
            TriangleMesh::Layout layout;
            uint16_t curr_strip = 0;
            // points
            {
                  TriangleMesh::LayoutItem meshLayout = {};
                  meshLayout.type = TriangleMesh::ArrayType::ARRAY_VERTEX;
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
                  TriangleMesh::LayoutItem normLayout = {};
                  normLayout.type = TriangleMesh::ArrayType::ARRAY_NORMAL;
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
                        TriangleMesh::LayoutItem texcoordLayout;
                        texcoordLayout.type = TriangleMesh::ArrayType::ARRAY_TEX_UV;
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
            for (auto& item : layout) {
                  item.strip = curr_strip;
                  char* p_char = (char*)raw_data + item.offset;
                  for (int i = 0; i < vertex_count; i++) {
                        std::memcpy(p_char, item.data_ptr, item.e_size*item.e_count);
                        p_char += item.strip;
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
            TriangleMesh* tri_mesh = new TriangleMesh(raw_data, layout, curr_strip*vertex_count, vertex_count,
                  idx_data, mesh->mNumFaces, GL_UNSIGNED_INT, obj2world);
            meshes.push_back(tri_mesh);
      }
      for (int i = 0; i < node->mNumChildren; i++) {
            addMesh(node->mChildren[i], scene, o2w, meshes);
      }
}

std::vector<TriangleMesh*> LoadMeshes(const aiScene* scene) {
      if (scene->mRootNode->mNumMeshes == 0)
            return {};
      std::vector<TriangleMesh*> meshes;
      addMesh(scene->mRootNode, scene, aiMatrix4x4(), meshes);
      return meshes;
}

static std::map<TriangleMesh::ArrayType, uint16_t> ShaderLocMap = {
      {TriangleMesh::ARRAY_VERTEX, 0},
      {TriangleMesh::ARRAY_TEX_UV, 1},
      {TriangleMesh::ARRAY_NORMAL, 2},
};

void TriangleMesh::load(QOpenGLExtraFunctions* f) {
      f->glGenVertexArrays(1, &_vao);
      f->glBindVertexArray(_vao);
      // vbo
      f->glGenBuffers(1, &_vbo);
      f->glBindBuffer(GL_ARRAY_BUFFER, _vbo);
      f->glBufferData(GL_ARRAY_BUFFER, vbuffer_size, vertex_data, GL_STATIC_DRAW);
      // ebo
      f->glGenBuffers(1, &_ebo);
      f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
      // each int32_t contains 4 bytes
      f->glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int32_t) * 3 * index_num, index_data, GL_STATIC_DRAW);
      // configure vertex pointers (stored in vao)
      for (auto& l : layout) {
            if (ShaderLocMap.find(l.type) == ShaderLocMap.end())
                  // not yet configured in vertex shader
                  continue;
            uint16_t shaderLoc = ShaderLocMap[l.type];
            f->glVertexAttribPointer(shaderLoc, l.e_count, l.e_format, l.normalized, l.strip, (void*)l.offset);
            f->glEnableVertexAttribArray(shaderLoc);
      }

      f->glBindVertexArray(0);
      f->glBindBuffer(GL_ARRAY_BUFFER, 0);
      f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}