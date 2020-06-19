#include "core/primitive.h"
#include <glad/glad.h>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include "main/renderworker.h"

static uint16_t pID_counter = 0;

PrimitiveBase::PrimitiveBase(const Transform& obj2world) : _obj2world(obj2world) {
      pID = ++pID_counter;
}

PrimitiveBase::PrimitiveBase()  {
      pID = ++pID_counter;
}

void PrimitiveBase::drawReference()
{
      glPointSize(3.0f);
      Shader* pointShader = LoadShader(ShaderType::POINT, true);
      pointShader->use();
      pointShader->setUniformF("obj2world", _obj2world.getRowMajorData());
      const Matrix4& w2c = RenderWorker::getCamera()->world2cam();
      const Matrix4& c2ndc = RenderWorker::getCamera()->Cam2NDC();
      pointShader->setUniformF("world2cam", &w2c);
      pointShader->setUniformF("cam2ndc", &c2ndc);
      glDrawArrays(GL_POINTS, 0, 1);
}

Primitive3D* CreatePrimitiveFromMeshes(TriangleMesh* mesh) {
      Primitive3D* p = new Primitive3D(nullptr, nullptr);
      //p->setMesh(mesh);
      return p;
}
// Primitive* CreatePrimitiveFromModelFile(std::string path) {
//       Assimp::Importer importer;
//       const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);
//       std::vector<TriangleMesh*> meshes = LoadMeshes(scene);
//       // TODO: maybe other stuff(texture map info, etc.) could be load together
//       Primitive* p = new Primitive(nullptr, nullptr);
//       p->setMeshes(meshes);
//       return p;
// }

void Primitive3D::load() {
      for (auto* mesh : _meshes) {
            mesh->load();
      }
      // TODO: consider other types of RTMaterial
      for (auto& m : rt_m)
            m.load();
      loaded = true;
}

void Primitive3D::drawPrepare(Shader* shader, int meshIndex) {
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
void Primitive3D::draw(Shader* shader) {
      // draw all meshes
      // TODO: maybe different meshes' materials/textures are different.
      for (int i = 0; i < _meshes.size(); i++) {
            PBRMaterial& mtl = rt_m[i];
            shader->setUniformF("globalEmission", mtl.globalEmission()[0], mtl.globalEmission()[1], mtl.globalEmission()[2]);
            shader->setUniformF("albedoFactor", mtl.albedoFactor.rgb[0], mtl.albedoFactor.rgb[1], mtl.albedoFactor.rgb[2]);
            shader->setUniformF("emissiveFactor", mtl.emissiveFactor.rgb[0], mtl.emissiveFactor.rgb[1], mtl.emissiveFactor.rgb[2]);
            shader->setUniformF("metallicFactor", mtl.metallicFactor);
            shader->setUniformF("roughFactor", mtl.roughFactor);
            shader->setUniformUI("primitiveID", pID);
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
            glDrawElements(m->primitiveMode(), 3 * m->face_count(), m->indexElementT(), 0);
      }
      if (drawReferencePoint)
            drawReference();
      glBindVertexArray(0);
}

void Primitive3D::drawSimple(Shader* shader) {
      for (int i = 0; i < _meshes.size(); i++) {
            auto& m = _meshes[i];
            glBindVertexArray(m->vao());
            shader->setUniformF("obj2world", _obj2world.getRowMajorData());
            glDrawElements(m->primitiveMode(), 3 * m->face_count(), m->indexElementT(), 0);
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

void Primitive2D::draw(Shader* s)
{
      //s = LoadShader(CHAR_2D, true);
      s->use();
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, image.tbo());
      // might be vertex attributes
      //_obj2world.moveTo(posWorld.x, posWorld.y, posWorld.z);
      /*_obj2world.m[0][3] = posWorld.x;
      _obj2world.m[1][3] = posWorld.y;
      _obj2world.m[2][3] = posWorld.z;*/
      s->setUniformF("obj2world", _obj2world.getRowMajorData());
      s->setUniformF("size", size.x, size.y);
      // same for all 2d primitives
      const Matrix4& w2c = RenderWorker::getCamera()->world2cam();
      const Matrix4& c2ndc = RenderWorker::getCamera()->Cam2NDC();
      s->setUniformF("world2cam", &w2c);
      s->setUniformF("cam2ndc", &c2ndc);
      // vao doesn't matter
      TriangleMesh::screenMesh.glUse();
      // draw a point
      uint32_t e = glGetError();
      glDrawArrays(GL_POINTS, 0, 1);
      e = glGetError();
      e = glGetError();
      if (drawReferencePoint)
            drawReference();
}

void Primitive2D::load() {
      if (image.ready2Load())
            image.load();
}