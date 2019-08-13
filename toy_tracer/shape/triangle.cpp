#include "shape/triangle.h"
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

TriangleMesh* LoadTriangleMesh(const std::string& path) {
      Assimp::Importer importer;
      const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);
      if (scene->mRootNode->mNumMeshes > 0) {
            aiMesh* mesh = scene->mMeshes[scene->mRootNode->mMeshes[0]];
            
      }
      return nullptr;
}