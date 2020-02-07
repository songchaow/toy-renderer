#include "core/primitive.h"
#include <glad/glad.h>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include "main/renderworker.h"

Primitive* CreatePrimitiveFromMeshes(TriangleMesh* mesh) {
      Primitive* p = new Primitive(nullptr, nullptr);
      //p->setMesh(mesh);
      return p;
}
Primitive* CreatePrimitiveFromModelFile(std::string path) {
      Assimp::Importer importer;
      const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);
      std::vector<TriangleMesh*> meshes = LoadMeshes(scene);
      // TODO: maybe other stuff(texture map info, etc.) could be load together
      Primitive* p = new Primitive(nullptr, nullptr);
      p->setMeshes(meshes);
      return p;
}

void Primitive::load() {
      for (auto* mesh : _meshes) {
            mesh->load();
      }
      // TODO: consider other types of RTMaterial
      for (auto& m : rt_m)
            m.load();
}

void Primitive::draw(Shader* shader) {
      // draw all meshes
      // TODO: maybe different meshes' materials/textures are different.
      for (int i = 0; i < _meshes.size(); i++) {
            PBRMaterial& mtl = rt_m[i];
            shader->setUniformF("globalEmission", mtl.globalEmission()[0], mtl.globalEmission()[1], mtl.globalEmission()[2]);
            if (mtl.albedo_map.isLoad()) {
                  glActiveTexture(GL_TEXTURE0);
                  glBindTexture(GL_TEXTURE_2D, mtl.albedo_map.tbo());
            }
            if (mtl.metallicRoughnessMap.isLoad()) {
                  glActiveTexture(GL_TEXTURE1);
                  glBindTexture(GL_TEXTURE_2D, mtl.metallicRoughnessMap.tbo());
            }
            if (mtl.normal_map.isLoad()) {
                  glActiveTexture(GL_TEXTURE2);
                  glBindTexture(GL_TEXTURE_2D, mtl.normal_map.tbo());
            }
            if (mtl.emissive_map.isLoad()) {
                  glActiveTexture(GL_TEXTURE3);
                  glBindTexture(GL_TEXTURE_2D, mtl.emissive_map.tbo());
            }
            auto& m = _meshes[i];
            glBindVertexArray(m->vao());
            shader->setUniformF("obj2world", _obj2world.getRowMajorData());
            // no need to bind the ebo again
            // eg: 2 faces => 6 element count
            GLenum err = glGetError();
            glDrawElements(GL_TRIANGLES, 3 * m->face_count(), m->indexElementT(), 0);
            err = glGetError();
      }
      
}