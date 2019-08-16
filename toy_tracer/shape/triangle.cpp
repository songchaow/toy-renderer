#include "shape/triangle.h"
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include <assimp/vector3.h>
#include <cstring>

TriangleMesh* LoadTriangleMesh(const std::string& path) {
      Assimp::Importer importer;
      const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);
      TriangleMesh::Layout layout;
      uint16_t curr_strip = 0;
      if (scene->mRootNode->mNumMeshes > 0) {
            unsigned int mesh_count = scene->mNumMeshes;
            // now only fetch the first mesh
            aiMesh* mesh = scene->mMeshes[scene->mRootNode->mMeshes[0]];
            unsigned int vertex_count = mesh->mNumVertices;
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
            TriangleMesh* tri_mesh = new TriangleMesh(raw_data, layout);
            return tri_mesh;
      }

      return nullptr;
}