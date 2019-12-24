#include "core/primitive.h"
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

void Primitive::load(QOpenGLFunctions_4_0_Core* f) {
      for (auto* mesh : _meshes) {
            mesh->load(f);
      }
      // TODO: consider other types of RTMaterial
      rt_m.load(f);
      shader = rt_m.shader();
}

void Primitive::draw(QOpenGLFunctions_4_0_Core* f) {
      // draw all meshes
      // TODO: maybe different meshes' materials/textures are different.
      shader->use();
      // set lights
      uint16_t startPos = shader->getUniformLocation("pointLights[0].pos");
      const std::vector<PointLight*>& pointLights = RenderWorker::Instance()->pointLights();
      for (int i = 0; i < pointLights.size(); i++) {
            shader->setUniformF(startPos++, pointLights[i]->pos().x, pointLights[i]->pos().y, pointLights[i]->pos().z);
            
      }
      startPos += Shader::maxPointLightNum - pointLights.size();
      for (int i = 0; i < pointLights.size(); i++) {
            shader->setUniformF(startPos++, pointLights[i]->radiance().rgb[0], pointLights[i]->radiance().rgb[1], pointLights[i]->radiance().rgb[2]);
      }
      // set other lights' radiance to zero
      /*startPos += 1;
      for (int i = pointLights.size(); i < Shader::maxPointLightNum; i++) {
            shader->setUniformF(startPos, 0.f, 0.f, 0.f);
            startPos += 2;
      }*/
      shader->setUniformF("globalEmission", rt_m.globalEmission()[0], rt_m.globalEmission()[1], rt_m.globalEmission()[2]);
      shader->setUniformI("albedoSampler", 0);
      if (rt_m.albedo_map.isLoad()) {
            f->glActiveTexture(GL_TEXTURE0);
            f->glBindTexture(GL_TEXTURE_2D, rt_m.albedo_map.tbo());
      }
      shader->setUniformI("metallicSampler", 1);
      if (rt_m.metallic_map.isLoad()) {
            f->glActiveTexture(GL_TEXTURE1);
            f->glBindTexture(GL_TEXTURE_2D, rt_m.metallic_map.tbo());
      }
      shader->setUniformI("roughnessSampler", 2);
      if (rt_m.rough_map.isLoad()) {
            f->glActiveTexture(GL_TEXTURE2);
            f->glBindTexture(GL_TEXTURE_2D, rt_m.rough_map.tbo());
      }
      shader->setUniformI("emissionSampler", 3);
      shader->setUniformI("aoSampler", 4);
      // set camera
      shader->setUniformF("cam2ndc", RenderWorker::getCamera()->Cam2NDC().getRowMajorData());
      shader->setUniformF("camPos", RenderWorker::getCamera()->pos().x, RenderWorker::getCamera()->pos().y, RenderWorker::getCamera()->pos().z);
      for (auto& m : _meshes) {
            f->glBindVertexArray(m->vao());
            // set MVP
            // TODO: obj2world could be stored for each mesh's shader program
            shader->setUniformF("obj2world", _obj2world.getRowMajorData());
            shader->setUniformF("world2cam", RenderWorker::getCamera()->world2cam().getRowMajorData());
            
            // no need to bind the ebo again
            // eg: 2 faces => 6 element count
            GLenum err = f->glGetError();
            f->glDrawElements(GL_TRIANGLES, 3 * m->face_count()*2, GL_UNSIGNED_INT, 0);
            err = f->glGetError();
      }
}