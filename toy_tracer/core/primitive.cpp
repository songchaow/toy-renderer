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

void Primitive::drawPrepare(Shader* shader, int meshIndex) {
      PBRMaterial& mtl = rt_m[meshIndex];
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
      auto& m = _meshes[meshIndex];
      glBindVertexArray(m->vao());
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
            glDrawElements(m->primitiveMode(), 3 * m->face_count(), m->indexElementT(), 0);
            err = glGetError();
      }
      glBindVertexArray(0);
}

void InstancedPrimitive::GenInstancedArray() {
      glGenBuffers(1, &iabo);
      glBindBuffer(GL_ARRAY_BUFFER, iabo);
      glBufferData(GL_ARRAY_BUFFER, sizeof(Matrix4)*obj2worlds.size(), obj2worlds.data(), GL_STATIC_DRAW);
      for (int i = 0; i < _meshes.size(); i++) {
            glBindVertexArray(_meshes[i]->vao());
            // bind instance array in addition to existing attributes
            glBindBuffer(GL_ARRAY_BUFFER, iabo);
            glEnableVertexAttribArray(4);
            glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Matrix4), 0 * 4 * sizeof(Float));
            glEnableVertexAttribArray(5);
            glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(Matrix4), (void*)(1 * 4 * sizeof(Float)));
            glEnableVertexAttribArray(6);
            glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Matrix4), (void*)(2 * 4 * sizeof(Float)));
            glEnableVertexAttribArray(7);
            glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(Matrix4), (void*)(3 * 4 * sizeof(Float)));
            glVertexAttribDivisor(4, 1);
            glVertexAttribDivisor(5, 1);
            glVertexAttribDivisor(6, 1);
            glVertexAttribDivisor(7, 1);
            glBindVertexArray(0);
      }
}

void InstancedPrimitive::draw(Shader* s) {
      for (int i = 0; i < _meshes.size(); i++) {
            drawPrepare(s, i);
            TriangleMesh* mesh = _meshes[i];
            glDrawElementsInstanced(mesh->primitiveMode(), mesh->face_count() * 3, mesh->indexElementT(), 0, obj2worlds.size());
            glBindVertexArray(0);
      }
}