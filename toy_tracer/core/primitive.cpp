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

void Primitive3D::calcAABB()
{
      for (auto* m : _meshes) {
            m->calcAABB();
            _aabb += m->aabb();
      }
}

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
      // TODO: maybe different meshes' materials/textures are different.]
      shader->use();
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
            
            // calc actual z range (optimize: if the object moves)
            // first calculate cam oriented bb
            _camAABB = (RenderWorker::Instance()->getCamera()->world2cam() * _obj2world.m)(_aabb);
            Point2f centerLocalXZ = m->cb().center;
            Float sizeWorld = _obj2world.srt.scaleX * m->cb().size;
            Point3f xzCam3 = RenderWorker::Instance()->getCamera()->world2cam()(_obj2world(Point3f(centerLocalXZ.x, 0, centerLocalXZ.y)));
            Float destinateZCam = xzCam3.z + sizeWorld * RenderWorker::Instance()->getCamera()->CosPitchAngle();
            shader->setUniformF("destinateZCam", destinateZCam);
            // flatten aabb
            _camAABB.pMax.z = destinateZCam + (_camAABB.pMax.z - destinateZCam) / flattenRatio;
            _camAABB.pMin.z = destinateZCam + (_camAABB.pMin.z - destinateZCam) / flattenRatio;

            // not used now
            Point2f centerxzCam = Point2f(xzCam3.x, xzCam3.z);
            centerxzCam[1] = centerxzCam[1] + sizeWorld * (1.0 - 1.0/flattenRatio);
            _zRange.center = centerxzCam;
            _zRange.axisX = sizeWorld;
            _zRange.axisY = sizeWorld / flattenRatio;

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

void FindIntersect(const Point3f& posCam1, const Point3f& posCam2, std::vector<Primitive3D*>& chars_inrange) {
      chars_inrange.clear();
      for (auto* c : RenderWorker::Instance()->scene().characters3D) {
            bool leftinRange = c->CamOrientedBB().In(posCam1);
            bool rightinRange = c->CamOrientedBB().In(posCam2);
            if (leftinRange || rightinRange) {
                  chars_inrange.push_back(c);
            }
      }
}

void Primitive2D::drawWithDynamicZ()
{
      // test intersection
      Point3f posCam = RenderWorker::Instance()->getCamera()->world2cam()(_obj2world.pos());
      Point3f posCamRightUp = posCam;
      posCamRightUp.x += size.x;
      posCamRightUp.y += size.y;
      Point2f xzCam = Point2f(posCam.x, posCam.z);
      Point2f xzCamRight = xzCam + Vector2f(size.x, 0);
      std::vector<Primitive3D*> chars_inrange;
      
      do {
            FindIntersect
      } while ();

      bool hasFrontLocation = false;
      for (auto* c : RenderWorker::Instance()->scene().characters3D) {
            CamOrientedEllipse::Location l;
            /*bool leftinRange = c->zRange().inRange(xzCam, l);
            bool rightinRange = c->zRange().inRange(xzCamRight, l);*/
            bool leftinRange = c->CamOrientedBB().In(posCam);
            bool rightinRange = c->CamOrientedBB().In(posCamRightUp);
            if (leftinRange || rightinRange) {
                  //if (l == CamOrientedEllipse::Location::FRONT)
                    //    hasFrontLocation = true;
                  chars_inrange.push_back(c);
                  hasFrontLocation = true;
            }
      }
      Shader* char2d_dynamicZ = LoadShader(ShaderType::CHAR_2D_NEW, true);
      char2d_dynamicZ->use();
      if (chars_inrange.empty())
            char2d_dynamicZ->setUniformBool("forceDepth", false);
      else {
            char2d_dynamicZ->setUniformBool("forceDepth", true);
            Float depthCam;
            if (RenderWorker::Instance()->put2dcharatFront) {
                  // currently set to the most front z
                  /*Float maxZ = chars_inrange[0]->zRange().FrontZ();
                  for (auto it = chars_inrange.begin() + 1; it < chars_inrange.end(); it++) {
                        if ((*it)->zRange().FrontZ() > maxZ)
                              maxZ = (*it)->zRange().FrontZ();
                  }
                  depthCam = maxZ;*/
                  Float maxZ = chars_inrange[0]->CamOrientedBB().pMax.z;
                  for (auto it = chars_inrange.begin() + 1; it < chars_inrange.end(); it++) {
                        if ((*it)->CamOrientedBB().pMax.z > maxZ)
                              maxZ = (*it)->CamOrientedBB().pMax.z;
                  }
                  depthCam = maxZ;
            }
            else {
                  /*Float minZ = chars_inrange[0]->zRange().FrontZ();
                  for (auto it = chars_inrange.begin() + 1; it < chars_inrange.end(); it++) {
                        if ((*it)->zRange().FrontZ() < minZ)
                              minZ = (*it)->zRange().FrontZ();
                  }
                  depthCam = minZ;*/
                  Float minZ = chars_inrange[0]->CamOrientedBB().pMin.z;
                  for (auto it = chars_inrange.begin() + 1; it < chars_inrange.end(); it++) {
                        if ((*it)->CamOrientedBB().pMin.z > minZ)
                              minZ = (*it)->CamOrientedBB().pMin.z;
                  }
                  depthCam = minZ;
            }
            char2d_dynamicZ->setUniformF("depthCam", depthCam);
      }
      draw(char2d_dynamicZ);
}

void Primitive2D::load() {
      if (image.ready2Load())
            image.load();
}